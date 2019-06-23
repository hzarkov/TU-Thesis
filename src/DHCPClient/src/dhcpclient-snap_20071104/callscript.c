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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>


#include "dhcpprotocol.h"

extern const char dhcpclient_version[];

static char numbuf[32];

#define setenvf(VARF, VAL, ...) do { sprintf(buf, VARF, ## __VA_ARGS__); setenv(buf, VAL, 1); } while(0)

static char *uintenv(const unsigned int arg) {
  sprintf(numbuf, "%u", arg);
  return numbuf;
}

static char *timeenv(const time_t *t) {
  char *rc = ctime(t);
  rc[strlen(rc)-1] = 0;
  return rc;
}

/* limit strings forwarded to script to RFC1123
   allowed characters and '_' */
static const char *sanedomainname(const char *hn) {
  const char *n = hn;
  char c;

  while((c = *n++))
    if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z') &&
	(c < '0' || c > '9') && c != '-' && c != '_' &&
	c != '.') return "";

  return hn;
}

static void cfgtoenv(const struct dhcp_config *cfg, const char *tag) {
  char buf[32];
  int n;

  setenvf("%sADDRESS", inet_ntoa(cfg->address), tag);
  setenvf("%sHOSTNAME", sanedomainname(cfg->hostname), tag);
  setenvf("%sDOMAINNAME", sanedomainname(cfg->domainname), tag);
  setenvf("%sSERVERADDR", inet_ntoa(cfg->dhcpd_addr), tag);
  setenvf("%sNETMASK", inet_ntoa(cfg->netmask), tag);
  setenvf("%sBROADCAST", inet_ntoa(cfg->broadcast), tag);
  setenvf("%sGATEWAY", inet_ntoa(cfg->gateway), tag);

  setenvf("%sDNS_NUM", uintenv(cfg->dns_num), tag);
  for(n=0; n<cfg->dns_num; n++)
    setenvf("%sDNS_%d", inet_ntoa(cfg->dns[n]), tag, n);
  
  setenvf("%sLEASE", uintenv(cfg->lease), tag);
  setenvf("%sT1", uintenv(cfg->t1), tag);
  setenvf("%sT2", uintenv(cfg->t2), tag);
}


int pt_callscript(const struct dhcp_interface *dhcpif, struct dhcp_config *oldcfg, dhcp_state newstate) {
  int status = -1;
  pid_t pid;

  if (!dhcpif->callscript_path) return 0;

  pid = fork();

  if (pid > 0) {
    (void)waitpid(pid, &status, 0);
  } else if (!pid) {
    cfgtoenv(&dhcpif->current_config, "DHCP_CURRENT_");
    cfgtoenv(&dhcpif->offered_config, "DHCP_OFFERED_");
    if (oldcfg) cfgtoenv(oldcfg, "DHCP_PREVIOUS_");

    setenv("DHCP_TO_LEASE", timeenv(&dhcpif->lease.tv_sec), 1);
    setenv("DHCP_TO_T1", timeenv(&dhcpif->t1.tv_sec), 1);
    setenv("DHCP_TO_T2", timeenv(&dhcpif->t2.tv_sec), 1);
    setenv("DHCP_VERSION", dhcpclient_version, 1);

    execl(dhcpif->callscript_path, dhcpif->callscript_path, 
	  dhcpif->ifname, dhcp_states[newstate], dhcp_states[dhcpif->state], NULL);
    _exit(-1);
  }

  return status;
}
