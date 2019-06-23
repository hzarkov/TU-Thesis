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


#include <assert.h>

#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <syslog.h>
#include <time.h>

#include "dhcpprotocol.h"
#include "dhcpmessage.h"
#include "netlink.h"
#include "sockets.h"
#include "arp.h"

const char *dhcp_states[] = {
  "REBOOTING",
  "OPER_DOWN",
  "ARP_CHECK_GATEWAY",
  "INIT_REBOOT",
  "INIT",
  "REQUESTING",
  "ARP_CHECK",
  "DECLINING",
  "BOUND",
  "RENEWING",
  "REBINDING",
  "RELEASING"
};

static const struct in_addr null_ip = { 0 };


int pt_opensockets(struct dhcp_interface *dhcpif) {
  (void)sk_openudp(dhcpif); /* Can live without it */
  return sk_openraw(dhcpif);
}


void pt_closesockets(struct dhcp_interface *dhcpif) {
  close(dhcpif->sock_udp);
  dhcpif->sock_udp = -1;
  close(dhcpif->sock_raw);
  dhcpif->sock_raw = -1;
}

/* List of dhcp options we request, must be kept in sync with
   what we require in parse_options() */
static char optlist[] = { BOOTP_OPTION_NETMASK, BOOTP_OPTION_GATEWAY, BOOTP_OPTION_DNS,
			  BOOTP_OPTION_HOSTNAME, BOOTP_OPTION_DOMAIN, BOOTP_OPTION_BROADCAST,
			  DHCP_OPTION_LEASE, DHCP_OPTION_T1, DHCP_OPTION_T2 };

int pt_sendmessage(struct dhcp_interface *dhcpif) {
  struct dhcp_message msg;
  struct in_addr ipaddr;
  unsigned int leasetime = ~0;
  unsigned char dhcptype;
  char clientid[7];

  dhcpif->xid = random();

  dm_init(&msg);
  msg.xid = dhcpif->xid;
  msg.op = BOOTP_OPCODE_REQUEST;
  msg.htype = 1;
  msg.hlen = 6;
  memcpy(msg.chaddr, dhcpif->ifhwaddr, 6);

  switch(dhcpif->state) {
  case INIT:
    // DHCP-Discover
    ipaddr.s_addr = 0;
    dhcptype = DHCP_TYPE_DISCOVER;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    break;
  case INIT_REBOOT:
    ipaddr = dhcpif->current_config.address;
    dhcptype = DHCP_TYPE_REQUEST;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    dm_add_option(&msg, DHCP_OPTION_REQADDR, 4, &dhcpif->offered_config.address.s_addr);
    break;
  case REQUESTING:
    ipaddr.s_addr = 0;
    dhcptype = DHCP_TYPE_REQUEST;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    dm_add_option(&msg, DHCP_OPTION_REQADDR, 4, &dhcpif->offered_config.address.s_addr);
    dm_add_option(&msg, DHCP_OPTION_SERVER, 4, &dhcpif->offered_config.dhcpd_addr.s_addr);
    break;
  case RENEWING:
    ipaddr = dhcpif->current_config.dhcpd_addr;
    dhcptype = DHCP_TYPE_REQUEST;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    msg.ciaddr = dhcpif->current_config.address.s_addr;
    break;
  case REBINDING:
    ipaddr = dhcpif->current_config.address;
    dhcptype = DHCP_TYPE_REQUEST;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    msg.ciaddr = dhcpif->current_config.address.s_addr;
    break;
  case RELEASING:
    ipaddr = dhcpif->current_config.dhcpd_addr;
    dhcptype = DHCP_TYPE_RELEASE;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    msg.ciaddr = dhcpif->current_config.address.s_addr;
    break;
  case DECLINING:
    ipaddr.s_addr = 0;
    dhcptype = DHCP_TYPE_DECLINE;
    dm_add_option(&msg, DHCP_OPTION_TYPE, 1, &dhcptype);
    dm_add_option(&msg, DHCP_OPTION_REQADDR, 4, &dhcpif->offered_config.address.s_addr);
    dm_add_option(&msg, DHCP_OPTION_SERVER, 4, &dhcpif->offered_config.dhcpd_addr.s_addr);
    break;
  case REBOOTING:
  case OPER_DOWN:
  case ARP_CHECK_GATEWAY:
  case BOUND:
  case ARP_CHECK:
    return -1;
  }

  /* Making these options depending on message type instead of
     statemachine seems shorter and more readable */
  if (dhcptype == DHCP_TYPE_DISCOVER || dhcptype == DHCP_TYPE_REQUEST) {
    dm_add_option(&msg, DHCP_OPTION_OPTIONREQ, sizeof(optlist), optlist);
    dm_add_option(&msg, DHCP_OPTION_LEASE, sizeof(leasetime), &leasetime);
    dm_add_option(&msg, BOOTP_OPTION_HOSTNAME, dhcpif->hostname_len, dhcpif->hostname);
  }

  clientid[0] = 1; /* Hardware type ethernet */
  memcpy(clientid+1, dhcpif->ifhwaddr, 6);
  dm_add_option(&msg, DHCP_OPTION_CLIENT_IDENTIFIER, 7, clientid);

  dm_finish_options(&msg);

  if (dhcpif->sock_udp != -1 &&
      (dhcpif->state == RENEWING || dhcpif->state == RELEASING))
    return dm_send_msg(dhcpif->sock_udp, &ipaddr, &msg);
  else
    return dm_broadcast_msg_raw(dhcpif->sock_raw, ipaddr, dhcpif->ifidx, &msg);
}

/* Inspired from asm-generic/unaligned.h. gcc3.3.5 and 4.1.2
 * optimize well on i386 */
struct dhcp_u32 { u_int32_t x __attribute__((packed)); };
static inline u_int32_t get_unaligned32(const u_int32_t *addr)
{
	const struct dhcp_u32 *ptr = (const struct dhcp_u32 *) addr;
	return ptr->x;
}

static void parse_options(struct dhcp_message *msg, struct dhcp_config *cfg) {
  u_int8_t *opt;

  memset(cfg, 0, sizeof(*cfg));

  cfg->address.s_addr = msg->yiaddr;
  cfg->dhcpd_addr.s_addr = msg->siaddr;
  while((opt = dm_next_option(msg))) {
    u_int8_t *optdata = opt+2;
    u_int8_t optsize = *(opt+1);
    u_int8_t m;

    /* printf("Option = %d\n", *opt); */

    switch(*opt) {
    case DHCP_OPTION_TYPE:
      if (optsize == 1)
	cfg->dhcpmsgtype = *optdata;
      break;
    case DHCP_OPTION_SERVER:
      if (optsize == 4)
	cfg->dhcpd_addr.s_addr = get_unaligned32((u_int32_t *)optdata);
      break;
    case BOOTP_OPTION_NETMASK:
      if (optsize == 4)
	cfg->netmask.s_addr = get_unaligned32((u_int32_t *)optdata);
      break;
    case BOOTP_OPTION_GATEWAY:
      if (optsize >= 4)
	cfg->gateway.s_addr = get_unaligned32((u_int32_t *)optdata);
      break;
    case BOOTP_OPTION_DNS:
      if (!(optsize & 3)) {
	u_int8_t n;

	m = optsize / 4;
	if (m > MAXOPTS) m = MAXOPTS;
	cfg->dns_num = m;

	for (n=0; n<m; n++)
	  cfg->dns[n].s_addr = get_unaligned32((u_int32_t *)(optdata+4*n));
      }
      break;
    case BOOTP_OPTION_HOSTNAME:
      if (optsize >= sizeof(cfg->hostname)) optsize = sizeof(cfg->hostname)-1;
      memcpy(cfg->hostname, optdata, optsize);
      cfg->hostname[optsize] = 0;
      break;
    case BOOTP_OPTION_DOMAIN:
      if (optsize >= sizeof(cfg->domainname)) optsize = sizeof(cfg->domainname)-1;
      memcpy(cfg->domainname, optdata, optsize);
      cfg->domainname[optsize] = 0;
      break;
    case BOOTP_OPTION_BROADCAST:
      if (optsize == 4)
	cfg->broadcast.s_addr = get_unaligned32((u_int32_t *)optdata);
      break;
    case DHCP_OPTION_LEASE:
      if (optsize == 4)
	cfg->lease = ntohl(get_unaligned32((u_int32_t *)optdata));
      break;
    case DHCP_OPTION_OVERLOAD:
      if (optsize == 1 && *optdata <= DHCP_OVERLOAD_BOTH)
	msg->overload = *optdata;
      break;
    case DHCP_OPTION_T1:
      if (optsize == 4)
	cfg->t1 = ntohl(get_unaligned32((u_int32_t *)optdata));
      break;
    case DHCP_OPTION_T2:
      if (optsize == 4)
	cfg->t2 = ntohl(get_unaligned32((u_int32_t *)optdata));
      break;
    }
  }
}


static void dhcp_backoff(struct timeval *src, struct timeval *dst, int cnt) {
  struct timeval add;

  if (cnt > 5) cnt = 5;

  add.tv_sec = 2;
  for(; cnt; --cnt) add.tv_sec *= 2;

  add.tv_usec = 1000000 - random() % 2000 * 1000;
  if (add.tv_usec < 0) {
    add.tv_usec += 1000000;
    add.tv_sec--;
  }

  timeradd(src, &add, dst);
}


static void dhcp_backoff_middle(const struct timeval *now, const struct timeval *to,
				struct timeval *dst) {
  struct timeval diff;

  /* RFC2131: timeout should be half of the remaining time, but at least
     60 seconds. dhcpclient: avoid timeout if state would be switched anyway
     6 seconds later. */
  timersub(to, now, &diff);
  if (diff.tv_sec <= 65) {
    *dst = *to;
  } else {
    if (diff.tv_sec & 1) {
      diff.tv_sec--;
      diff.tv_usec += 1000000;
    }
    diff.tv_sec >>= 1;
    diff.tv_usec >>= 1;
    if (diff.tv_sec < 60) {
      diff.tv_sec = 60;
      diff.tv_usec = 0;
    }
    timeradd(now, &diff, dst);
  }
}


int pt_recvmessage_dhcp(struct dhcp_interface *dhcpif, char *dframe, int plen) {
  struct dhcp_message msg;
  struct dhcp_config opts;
  struct in_addr srcaddr;

  if (dm_parse_msg_raw(dframe, plen, &srcaddr, &msg)) return 0;

  if (msg.xid != dhcpif->xid ||
      msg.hlen != 6 ||
      memcmp(msg.chaddr, dhcpif->ifhwaddr,6)) return 0; // Message not meant for us

  parse_options(&msg, &opts);
  gettimeofday(&opts.recvtime, NULL);
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(opts.dhcpd_addr.s_addr), str, INET_ADDRSTRLEN);
  printf("%s",str);

  inet_ntop(AF_INET, &(opts.address.s_addr), str, INET_ADDRSTRLEN);
  printf(",%s",str);

  inet_ntop(AF_INET, &(opts.netmask.s_addr), str, INET_ADDRSTRLEN);
  printf(",%s",str);

  inet_ntop(AF_INET, &(opts.broadcast.s_addr), str, INET_ADDRSTRLEN);
  printf(",%s",str);

  inet_ntop(AF_INET, &(opts.gateway.s_addr), str, INET_ADDRSTRLEN);
  printf(",%s",str);

  printf(",%u",opts.lease);
  
  for(int i=0; i<opts.dns_num;i++)
  {
    inet_ntop(AF_INET, &(opts.dns[i].s_addr), str, INET_ADDRSTRLEN);
    printf(",%s",str);
  }
  printf("\n");

  switch(dhcpif->state) {
  case INIT:
    if (opts.dhcpmsgtype != DHCP_TYPE_OFFER) return 0; // does not match state
    dhcpif->offered_config = opts;
    pt_changestate(dhcpif, REQUESTING);
    (void)pt_sendmessage(dhcpif);
    dhcp_backoff(&opts.recvtime, &dhcpif->to_event, ++dhcpif->cnt);
    break;
  case REQUESTING:
  case RENEWING:
  case REBINDING:
  case INIT_REBOOT:
    switch(opts.dhcpmsgtype) {
    case DHCP_TYPE_OFFER:
    default:
      break;
    case DHCP_TYPE_ACK:
      if (dhcpif->arpcheck && dhcpif->state == REQUESTING) {
	dhcpif->offered_config = opts;
	pt_changestate(dhcpif, ARP_CHECK);
	ar_send_query_ip(dhcpif->sock_raw, dhcpif->ifidx, null_ip, dhcpif->ifhwaddr, opts.address, NULL);
	dhcpif->to_event.tv_sec = 0; dhcpif->to_event.tv_usec = 500000;
	timeradd(&opts.recvtime, &dhcpif->to_event, &dhcpif->to_event);
      } else {
	dhcpif->offered_config = opts;
	pt_changestate(dhcpif, BOUND);
	dhcpif->to_event = dhcpif->t1;
      }
      break;
    case DHCP_TYPE_NAK:
      pt_changestate(dhcpif, INIT);
      timerclear(&dhcpif->to_event); /* If we are NAKed, retry ASAP! */
      break;
    }
    break;
  default:
    break;
  }

  exit(0);
  return 0;
}

static void pt_recvmessage_arp(struct dhcp_interface *dhcpif, unsigned char pkttype,
			       char *dframe, int plen) {
  struct in_addr sip, dip;
  unsigned char smac[6], dmac[6];

  if (ar_parse_reply(dframe, plen, smac, &sip, dmac, &dip)) return;

  switch(dhcpif->state) {
  case ARP_CHECK:
    /* The answer we receive may have a differend dmac than our MAC
       as we queried with a cleared IP address. So accept the packet
       if it is destined to our host */
    if (pkttype == PACKET_HOST &&
	sip.s_addr == dhcpif->offered_config.address.s_addr) {
      pt_changestate(dhcpif, DECLINING);
      (void)pt_sendmessage(dhcpif);
      pt_changestate(dhcpif, INIT);
      gettimeofday(&dhcpif->to_event, NULL);
      dhcpif->to_event.tv_sec += 10; /* RFC2131 retry delay */
    }
    break;
  case ARP_CHECK_GATEWAY:
    /* If we receive an answer from the default gateway to us,
       everything is fine and we can go to BOUND */
    if (dhcpif->current_config.gateway.s_addr == sip.s_addr &&
	dhcpif->current_config.address.s_addr == dip.s_addr &&
	!memcmp(dhcpif->defgw_hwaddr, smac, ETH_ALEN) &&
	!memcmp(dhcpif->ifhwaddr, dmac, ETH_ALEN)) {
      pt_changestate(dhcpif, BOUND);
      dhcpif->to_event = dhcpif->t1;
    }
    break;
  case BOUND:
    /* We are waiting to learn the MAC address of our default
       gateway. Afterwards close sockets */
    if (dhcpif->current_config.gateway.s_addr == sip.s_addr &&
	dhcpif->current_config.address.s_addr == dip.s_addr &&
	!memcmp(dhcpif->ifhwaddr, dmac, ETH_ALEN)) {
      memcpy(dhcpif->defgw_hwaddr, smac, ETH_ALEN);
      pt_closesockets(dhcpif);
    }
    break;
  case REBOOTING:
  case OPER_DOWN:
  case INIT_REBOOT:
  case INIT:
  case REQUESTING:
  case DECLINING:
  case RENEWING:
  case REBINDING:
  case RELEASING:
    break;
  }
}


int pt_recvmessage(struct dhcp_interface *dhcpif) {
  char dframe[ETH_DATA_LEN];
  struct sockaddr_ll addr;
  socklen_t addrsize = sizeof(addr);
  ssize_t plen;

  plen = recvfrom(dhcpif->sock_raw, dframe, sizeof(dframe), 0,
		  (struct sockaddr *)&addr, &addrsize);

  if (plen == -1) return 0;

  switch(ntohs(addr.sll_protocol)) { /* FIXME: move htons into case-statements */
  case ETH_P_IP:
    return(pt_recvmessage_dhcp(dhcpif, dframe, plen));
  case ETH_P_ARP:
    pt_recvmessage_arp(dhcpif, addr.sll_pkttype, dframe, plen);
    break;
  }

  return 0;
}


void pt_changestate(struct dhcp_interface *dhcpif, dhcp_state newstate) {
  struct dhcp_config oldcfg; /* make old config available in BOUND state */
  struct timeval now = { 0, 0 };
  int n = 0;

  if (dhcpif->sock_raw == -1 && pt_opensockets(dhcpif)) {
    perror("Cannot open raw socket");
    exit(1);
  }

#ifdef USE_SYSLOG
  syslog(LOG_INFO, "state change from %s to %s",
	 dhcp_states[dhcpif->state], dhcp_states[newstate]);
#endif

  switch(newstate) {
  case INIT:
    dhcpif->t1 = dhcpif->t2 = dhcpif->lease = now;
    memset(&dhcpif->current_config, 0, sizeof(dhcpif->current_config));
    if (dhcpif->set_addr) nl_flushif(dhcpif);
    pt_callscript(dhcpif, NULL, INIT);
    break;

  case BOUND:
    /* change interface/route only if offered config really changed
       or after falling through INIT */
    if (dhcpif->current_config.address.s_addr != dhcpif->offered_config.address.s_addr ||
	dhcpif->current_config.netmask.s_addr != dhcpif->offered_config.netmask.s_addr ||
	dhcpif->current_config.broadcast.s_addr != dhcpif->offered_config.broadcast.s_addr ||
	dhcpif->current_config.gateway.s_addr != dhcpif->offered_config.gateway.s_addr)
      n = 1;
    gettimeofday(&now, NULL);
    oldcfg = dhcpif->current_config;
    dhcpif->current_config = dhcpif->offered_config;
    /* FIXME: validity check */
    dhcpif->t1 = dhcpif->t2 = dhcpif->lease = dhcpif->current_config.recvtime;
    dhcpif->lease.tv_sec += dhcpif->current_config.lease;
    dhcpif->t1.tv_sec += dhcpif->current_config.t1?dhcpif->current_config.t1:dhcpif->current_config.lease/2;
    dhcpif->t2.tv_sec += dhcpif->current_config.t2?dhcpif->current_config.t2:
      dhcpif->current_config.lease-dhcpif->current_config.lease/8; /* multiply with .875 */
    /* randomize T1 / T2 */
    if (dhcpif->current_config.lease > 5) {
      int diff = 5000 - random() % 10000;
      if (diff > 0) {
	struct timeval ta = { diff / 1000, diff % 1000 * 1000 };
	timeradd(&dhcpif->t1, &ta, &dhcpif->t1);
	timeradd(&dhcpif->t2, &ta, &dhcpif->t2);
      } else if (diff < 0) {
	struct timeval ts = { (-diff) / 1000, (-diff) % 1000 * 1000 };
	timersub(&dhcpif->t1, &ts, &dhcpif->t1);
	timersub(&dhcpif->t2, &ts, &dhcpif->t2);
      }
    }
    /* sanitize */
    if (sizeof(dhcpif->lease.tv_sec) == 4) {
      /* On 32 bit systems timers can overflow given a large lease time,
	 t1 or t2. The following check also covers the infinity value good
	 enough so that we can avoid special cases in the rest of the code.
      */
      if (timercmp(&dhcpif->lease, &dhcpif->current_config.recvtime, <))
	{ dhcpif->lease.tv_sec = INT_MAX; dhcpif->lease.tv_usec = 999999; }
      if (timercmp(&dhcpif->t1, &dhcpif->current_config.recvtime, <))
	{ dhcpif->t1.tv_sec = INT_MAX; dhcpif->t1.tv_usec = 999999; }
      if (timercmp(&dhcpif->t2, &dhcpif->current_config.recvtime, <))
	{ dhcpif->t2.tv_sec = INT_MAX; dhcpif->t2.tv_usec = 999999; }
    }
    if (timercmp(&dhcpif->t2, &dhcpif->lease, >)) dhcpif->t2 = dhcpif->lease;
    if (timercmp(&dhcpif->t1, &dhcpif->t2, >)) dhcpif->t1 = dhcpif->t2;
    if (n && dhcpif->set_addr) {
      (void)nl_configureif(dhcpif);  /* FIXME: check for error? */
      if (dhcpif->set_gateway && dhcpif->current_config.gateway.s_addr)
	nl_configuregw(dhcpif);
    }
    if (dhcpif->current_config.gateway.s_addr && dhcpif->arpcheck_gateway &&
	dhcpif->state != ARP_CHECK_GATEWAY) {
      /* Want to know ARP address of our default gateway. Just query
	 once without timeout, even though the IP stack will ask sooner
	 or later anyway */
      ar_send_query_ip(dhcpif->sock_raw, dhcpif->ifidx, dhcpif->current_config.address,
		       dhcpif->ifhwaddr, dhcpif->current_config.gateway, NULL);
      dhcpif->defgw_hwaddr[0] = 255; /* mark as invalid */
    } else
      pt_closesockets(dhcpif);
#ifdef USE_SYSLOG
    syslog(LOG_NOTICE, "Bound address %s, renewing at %s",
	   inet_ntoa(dhcpif->current_config.address), ctime(&dhcpif->t1.tv_sec));
#endif
    pt_callscript(dhcpif, &oldcfg, BOUND);
    break;

  case OPER_DOWN:
  case ARP_CHECK_GATEWAY:
  case RENEWING:
  case REBINDING:
  case RELEASING:
  case INIT_REBOOT:
  case REQUESTING:
  case ARP_CHECK:
  case DECLINING:
    pt_callscript(dhcpif, NULL, newstate);
    break;

  case REBOOTING:
    break;
  }

  if (dhcpif->state != newstate) {
    dhcpif->state = newstate;
    dhcpif->cnt = 0;
  }
}


const struct timeval *pt_nexttimeout(const struct dhcp_interface *dhcpif) {
  switch(dhcpif->state) {
  case INIT:
  case INIT_REBOOT:
  case ARP_CHECK_GATEWAY:
  case REQUESTING:
  case RENEWING:
  case REBINDING:
  case BOUND:
  case ARP_CHECK:
    // printf("to_event = %d.%d\n", dhcpif->to_event.tv_sec, dhcpif->to_event.tv_usec);
    return &dhcpif->to_event;
  case OPER_DOWN:
  case REBOOTING:
  case RELEASING:
  case DECLINING:
    break;
  }
  return NULL;
}




void pt_handletimeout(struct dhcp_interface *dhcpif) {
  struct timeval now;

  gettimeofday(&now, NULL);
  if (timercmp(&now, &dhcpif->to_event, <)) return;

  switch(dhcpif->state) {
  case REBOOTING:
    dhcpif->to_event = now;
    dhcpif->to_event.tv_sec += 1;
    pt_changestate(dhcpif, INIT);
    break;
  case INIT:
    (void)pt_sendmessage(dhcpif);
    dhcp_backoff(&now, &dhcpif->to_event, ++dhcpif->cnt);
    break;
  case ARP_CHECK_GATEWAY:
    if (++dhcpif->cnt >= dhcpif->arpcheck_gateway) { // default gateway not there, query DHCP server
      pt_changestate(dhcpif, INIT_REBOOT);
      (void)pt_sendmessage(dhcpif);
      dhcp_backoff(&now, &dhcpif->to_event, ++dhcpif->cnt);
    } else { // next arp request
      ar_send_query_ip(dhcpif->sock_raw, dhcpif->ifidx, dhcpif->current_config.address, dhcpif->ifhwaddr,
		       dhcpif->current_config.gateway, dhcpif->defgw_hwaddr);
      dhcpif->to_event.tv_sec = 0; dhcpif->to_event.tv_usec = 500000;
      timeradd(&now, &dhcpif->to_event, &dhcpif->to_event);
    }
    break;
  case INIT_REBOOT:
    if (dhcpif->cnt) { /* retry only once to avoid using a bogus IP address */
      pt_changestate(dhcpif, INIT);
    } else {
      (void)pt_sendmessage(dhcpif);
      dhcp_backoff(&now, &dhcpif->to_event, ++dhcpif->cnt);
    }
    break;
  case REQUESTING:
    if (dhcpif->cnt < 3) {
      (void)pt_sendmessage(dhcpif);
    } else {
      pt_changestate(dhcpif, INIT); /* wait 4 sec until retry */
    }
    dhcp_backoff(&now, &dhcpif->to_event, ++dhcpif->cnt);
    break;
  case ARP_CHECK:
    if (++dhcpif->cnt >= dhcpif->arpcheck) { // all arp requests failed
      pt_changestate(dhcpif, BOUND);
      dhcpif->to_event = dhcpif->t1;
    } else { // next arp request
      ar_send_query_ip(dhcpif->sock_raw, dhcpif->ifidx, null_ip, dhcpif->ifhwaddr, dhcpif->offered_config.address, NULL);
      dhcpif->to_event.tv_sec = 0; dhcpif->to_event.tv_usec = 500000;
      timeradd(&now, &dhcpif->to_event, &dhcpif->to_event);
    }
    break;
  case BOUND:
    if (!timercmp(&now, &dhcpif->t1, <))
      pt_changestate(dhcpif, RENEWING); // fall through
    else
      break;
  case RENEWING:
    if (!timercmp(&now, &dhcpif->t2, <))
      pt_changestate(dhcpif, REBINDING); // fall through
    else {
      (void)pt_sendmessage(dhcpif);
      dhcp_backoff_middle(&now, &dhcpif->t2, &dhcpif->to_event);
      break;
    }
  case REBINDING:
    if (!timercmp(&now, &dhcpif->lease, <)) {
      pt_changestate(dhcpif, INIT);
      (void)pt_sendmessage(dhcpif);
      dhcp_backoff(&now, &dhcpif->to_event, ++dhcpif->cnt);
    } else {
      (void)pt_sendmessage(dhcpif);
      dhcp_backoff_middle(&now, &dhcpif->lease, &dhcpif->to_event);
    }
    break;
  case OPER_DOWN:
  case RELEASING:
  case DECLINING:
    break;
  }
}


void pt_operstate(struct dhcp_interface *dhcpif, int up) {
  struct timeval now;
  gettimeofday(&now, NULL);

  if (up && dhcpif->state == OPER_DOWN) {
    if (timerisset(&dhcpif->lease)) {
      /* Could have changed network, try to validate our default gateway
	 if sensible, or try to regain ip address via broadcast.
	 Avoid removing IP address from interface for a few seconds */
      if (dhcpif->arpcheck_gateway &&  dhcpif->current_config.gateway.s_addr && 
	  timercmp(&now, &dhcpif->t1, <) && now.tv_sec - dhcpif->t_bound_operdown.tv_sec < 60 &&
	  dhcpif->defgw_hwaddr[0] != 255) {
	pt_changestate(dhcpif, ARP_CHECK_GATEWAY);
	ar_send_query_ip(dhcpif->sock_raw, dhcpif->ifidx, dhcpif->current_config.address, dhcpif->ifhwaddr, 
			 dhcpif->current_config.gateway, dhcpif->defgw_hwaddr);
	dhcpif->to_event.tv_sec = 0; dhcpif->to_event.tv_usec = 500000;
	timeradd(&now, &dhcpif->to_event, &dhcpif->to_event);
      } else {
	pt_changestate(dhcpif, INIT_REBOOT);
	(void)pt_sendmessage(dhcpif);
	/* The switch may well lose our first packet, so we
	   retry after two seconds instead of four */
	dhcp_backoff(&now, &dhcpif->to_event, dhcpif->cnt);
      }
    } else { /* don't have any acquired lease */
      dhcpif->to_event = now;
      pt_changestate(dhcpif, INIT);
      (void)pt_sendmessage(dhcpif);
      dhcp_backoff(&now, &dhcpif->to_event, dhcpif->cnt);
    }
  } else if (!up && dhcpif->state != OPER_DOWN) {
    if (dhcpif->state == BOUND) dhcpif->t_bound_operdown = now;
    pt_changestate(dhcpif, OPER_DOWN);
  }
}

void pt_initreboot(struct dhcp_interface *dhcpif) {
  if (dhcpif->state == BOUND || dhcpif->state == REBINDING ||
      dhcpif->state == RENEWING) {
    struct timeval now;

    pt_changestate(dhcpif, INIT_REBOOT);
    (void)pt_sendmessage(dhcpif);
    gettimeofday(&now, NULL);
    dhcp_backoff(&now, &dhcpif->to_event, dhcpif->cnt);
  }
}

void pt_release(struct dhcp_interface *dhcpif) {
  if (dhcpif->state == BOUND) {
    pt_changestate(dhcpif, RELEASING);
    (void)pt_sendmessage(dhcpif);
    if (dhcpif->set_addr) nl_flushif(dhcpif);
  }
}

void pt_init(struct dhcp_interface *dhcpif) {
  memset(dhcpif, 0, sizeof(*dhcpif));
  dhcpif->sock_udp = -1;
  dhcpif->sock_raw = -1;
}
