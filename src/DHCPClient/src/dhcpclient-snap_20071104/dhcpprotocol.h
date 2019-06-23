#ifndef DHCPPROTOCOL_H
#define DHCPPROTOCOL_H

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

#include <limits.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>

#ifndef HOST_NAME_MAX
#warning HOST_NAME_MAX not defined, setting to 64 (using uclibc?)
#define HOST_NAME_MAX 64
#endif

/* DHCP client states mostly per RFC2131. We need an oper down
   state, but not two reboots. Different to RFC, our statemachine
   will go from REBOOTING to REBINDING to allow fast reconfig
   after a network change
*/
typedef enum {
  REBOOTING,
  OPER_DOWN,
  ARP_CHECK_GATEWAY,
  INIT_REBOOT,
  INIT,
  /* SELECTING, we use fastest server */
  REQUESTING,
  ARP_CHECK,
  DECLINING, /* Pseudo state to send decline message after ARP_CHECK */
  BOUND,
  RENEWING,
  REBINDING,
  RELEASING /* during shutdown */
} dhcp_state;

/* textual description of states */
extern const char *dhcp_states[];

#define MAXOPTS 3

/* config received from server, mostly via options */
struct dhcp_config {
  /* timestamp we received the message */
  struct timeval recvtime;

  /* server address */
  struct in_addr dhcpd_addr;

  /* parsed options */
  struct in_addr address;
  struct in_addr netmask;
  struct in_addr broadcast;

  struct in_addr gateway;

  struct in_addr dns[MAXOPTS];
  unsigned short dns_num;

  char hostname[HOST_NAME_MAX];
  char domainname[HOST_NAME_MAX];

  unsigned int t1;
  unsigned int t2;
  unsigned int lease;

  char dhcpmsgtype;
};


struct dhcp_interface {
  int sock_udp;
  int sock_raw;
  dhcp_state state;
  unsigned int cnt; /* retransmit counter */

  int ifidx; /* kernel interface index */
  char ifname[IFNAMSIZ];
  unsigned char ifhwaddr[6];

  unsigned char defgw_hwaddr[6]; /* dynamic, but not learned via DHCP */
  unsigned int xid;

  /* absolute expiration times */
  struct timeval to_event;
  struct timeval t1;
  struct timeval t2;
  struct timeval lease;
  /* time we went from BOUND to OPER_DOWN */
  struct timeval t_bound_operdown;

  struct dhcp_config offered_config;
  struct dhcp_config current_config;

  char *callscript_path;

  char hostname[HOST_NAME_MAX]; /* Our hostname as sent to server */
  unsigned char hostname_len;

  unsigned short arpcheck; // number of arp requests to send
  unsigned short arpcheck_gateway;
  unsigned int set_gateway:1;
  unsigned int set_addr:1;
};


int pt_opensockets(struct dhcp_interface *dhcpif);

void pt_closesockets(struct dhcp_interface *dhcpif);

/* init structure */
void pt_init(struct dhcp_interface *dhcpif);

/* send a DHCP message according to current state */
int pt_sendmessage(struct dhcp_interface *dhcpif);

int pt_recvmessage(struct dhcp_interface *dhcpif);


/* handle statemachine change */

void pt_changestate(struct dhcp_interface *dhcpif, dhcp_state newstate);

/* timeout handling: handle timeout, get next timeout */

const struct timeval *pt_nexttimeout(const struct dhcp_interface *dhcpif);

void pt_handletimeout(struct dhcp_interface *dhcpif);

/* signal operstate change */

void pt_operstate(struct dhcp_interface *dhcpif, int up);

/* request INIT_REBOOT from outside*/

void pt_initreboot(struct dhcp_interface *dhcpif);

/* request release from outside. May leave statemachine nonfunctional,
   call on shutdown only! */

void pt_release(struct dhcp_interface *dhcpif);

/* call script */

int pt_callscript(const struct dhcp_interface *dhcpif, struct dhcp_config *oldcfg, dhcp_state newstate);

#endif
