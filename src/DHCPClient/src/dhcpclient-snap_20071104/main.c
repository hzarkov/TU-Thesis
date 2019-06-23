/*
 * DHCP Client
 *
 * Copyright 2006, 2007 Stefan Rompf <sux@loplof.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 */

#include "netlink.h"
#include "dhcpprotocol.h"
#include "dhcpmessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>


const char dhcpclient_version[] = DHCPCLIENT_VERSION;

#ifdef USE_SYSLOG
static char syslogname[64];
#endif

static struct dhcp_interface dif;

static nl_ostatus last_ostatus, ostatus;

static volatile int sigrecv;

/* signal handling is a bit racy, but don't wont to
   establish a pipe to ourself for sync delivery */
static void sighandler(int sig) {
  sigrecv = sig;
  signal(sig, sighandler);
}


static void setup_random() {
  int fd;
  int seed = time(NULL);

  fd = open("/dev/urandom", O_RDONLY);
  if (fd != -1) {
    (void)read(fd, &seed, sizeof(seed));
    close(fd);
  }
  srand(seed);
}


static int handle_ostatus() {
  last_ostatus = ostatus;
  switch(ostatus) {
  case OS_REMOVED:
    return 1;
  case OS_SHUT:
    return 1;
  case OS_DOWN:
    pt_operstate(&dif, 0);
    break;
  case OS_UP:
    pt_operstate(&dif, 1);
    break;
  }
  return 0;
}


static void usage(const char *path) {
  fprintf(stderr, "Usage: %s -T -G -c path -h hostname interface\n"
	  " -T : do not configure interface/route\n"
	  " -G : do not configure route\n"
	  " -c : path to helper script\n"
	  " -h : hostname to report\n"
	  " -a : Number of arp requests to send (0: disable)\n"
	  " -A : Number of arp requests to verify default gateway after OPER_UP (0: disable)\n"
	  " -v : display version and exit\n",
	  path);
  exit(1);
}


static int pidfile(const char *ifname, int operation) {
  char path[_POSIX_PATH_MAX];
  char pid[16];
  int fd, chars;

  if ((unsigned)snprintf(path, sizeof(path), DHCPCLIENT_PIDDIR "/dhcpclient-%s.pid", ifname) >= sizeof(path)) return -1;

  switch(operation) {
  case 1: /* create */
    fd = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) return -1;
    if (fcntl(fd, F_SETFD, FD_CLOEXEC)) return -1;
    if (flock(fd, LOCK_EX | LOCK_NB)) return -1;
    chars = sprintf(pid, "%d\n", getpid());
    write(fd, pid, chars);
    break; /* purposefully losing FD */
  case 2:
    unlink(path);
    break;
  }

  return 0;
}


int main(int argc, char **argv) {
  setup_random();
  pt_init(&dif);

  dif.set_addr = 1;
  dif.set_gateway = 1;
  dif.arpcheck = 2;
  dif.arpcheck_gateway = 3;

  (void)gethostname(dif.hostname, sizeof(dif.hostname));
  dif.callscript_path = DHCPCLIENT_SCRIPT;

  ostatus = OS_UP;

  {
    int opt;
    while((opt = getopt(argc, argv, "Tc:h:Ga:A:v")) != -1) {
      switch(opt) {
      case 'T':
	dif.set_addr = 0;
        break;
      case 'c':
        dif.callscript_path = optarg;
        break;
      case 'h':
	strncpy(dif.hostname, optarg, sizeof(dif.hostname));
        break;
      case 'G':
	dif.set_gateway = 0;
        break;
      case 'a':
	dif.arpcheck = atoi(optarg);
	break;
      case 'A':
	dif.arpcheck_gateway = atoi(optarg);
	break;
      case 'v':
	printf("dhcpclient version %s\n", dhcpclient_version);
	exit(0);
	break;
      default:
        usage(argv[0]);
      }
    }
  }

  if (optind >= argc) usage(argv[0]);

  dif.hostname[sizeof(dif.hostname)-1] = 0;
  dif.hostname_len = strlen(dif.hostname);

  signal(SIGHUP, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);

  (void)nl_open();
  if (nl_getifdata(argv[optind], &dif)) {
    fprintf(stderr, "%s: unknown or unusable interface %s\n", argv[0], argv[optind]);
    exit(1);
  }

  if (pidfile(dif.ifname, 1)) {
    perror("Cannot write pidfile");
    exit(1);
  }

//  printf("dhcpclient %s running on interface %s, index %d\n", dhcpclient_version, dif.ifname, dif.ifidx);

#ifdef USE_SYSLOG
  snprintf(syslogname, sizeof(syslogname), "%s/%s", argv[0], dif.ifname);
  openlog(syslogname, LOG_PID | LOG_NDELAY, LOG_DAEMON);
#endif

  if (handle_ostatus()) goto end;
  (void)pt_handletimeout(&dif);
  for(;;) {
    struct timeval to;
    const struct timeval *nto;
    fd_set readset;
    int cnt;

    FD_ZERO(&readset);
    FD_SET(nl_socket, &readset);
    if (dif.sock_udp != -1) FD_SET(dif.sock_udp, &readset);
    if (dif.sock_raw != -1) FD_SET(dif.sock_raw, &readset);

    nto = pt_nexttimeout(&dif);
    if (nto) {
      gettimeofday(&to, NULL);
      if (timercmp(&to, nto, >))
	timerclear(&to);
      else
	timersub(nto, &to, &to);
    }

    cnt = select(128, &readset, NULL, NULL, nto?&to:NULL); /* FIXME: max_fd */

    if (cnt < 0 && errno != EINTR) exit(1);

    switch(sigrecv) {
    case SIGHUP:
      sigrecv = 0;
      pt_initreboot(&dif);
      break;
    case SIGTERM:
    case SIGINT:
      goto end;
    }

    if (cnt < 0) continue;

    if (FD_ISSET(nl_socket, &readset))
      (void)nl_handleifmulticast();

    /* As we use one netlink socket only and have callbacks
       from pt_* to nl_*, synchronize operstate changes in
       main loop.
    */
    if (ostatus != last_ostatus && handle_ostatus()) goto end;

    if (dif.sock_udp != -1 && FD_ISSET(dif.sock_udp, &readset)) {
      /* we are not interested in responses we see on our udp socket */
      static char c[8];
      (void)read(dif.sock_udp, c, 8);
    }

    if (dif.sock_raw != -1 && FD_ISSET(dif.sock_raw, &readset))
      (void)pt_recvmessage(&dif);

    (void)pt_handletimeout(&dif);
  }

 end:
  pt_release(&dif);
  pidfile(dif.ifname, 2);
  return 0;
}


/*
 * At least in one place I want to break layering
 */
void nl_callback(int ifidx, nl_ostatus status) {
  if (dif.ifidx == ifidx) ostatus = status;
}
