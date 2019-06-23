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


#include <arpa/inet.h>
#include <assert.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <net/if.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>

#include "netlink.h"
#include "dhcpprotocol.h"


#define NLMSG_RECVSIZE 8192


int nl_socket = -1;

static unsigned int nl_seq;

/* internal callback handling */
static void (*nlcb_function)(struct nlmsghdr *msg, void **args);
static void *nlcb_args[3];
static __u32 nlcb_pid;
static unsigned int nlcb_seq;
static char nlcb_run;


int nl_open() {
 struct sockaddr_nl nlsock = {
    .nl_family = AF_NETLINK,
    .nl_pad = 0,
    .nl_pid = getpid(),
    .nl_groups = RTMGRP_LINK
  };

  nlcb_pid = nlsock.nl_pid;

  assert(nl_socket == -1);

  nl_socket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

  if (nl_socket == -1) return -1;

  if (bind(nl_socket, (const struct sockaddr *)&nlsock, sizeof(nlsock)))
    goto err_close;

  if (fcntl(nl_socket, F_SETFD, FD_CLOEXEC))
    goto err_close;

  return 0;

 err_close:
  nl_close();
  return -1;
}

void nl_close() {
  close(nl_socket);
  nl_socket = -1;
}


void nl_queryifstatus(int ifidx) {
  struct {
    struct nlmsghdr hdr;
    struct ifinfomsg ifinfo;
  } req;

  req.hdr.nlmsg_len = sizeof(req);
  req.hdr.nlmsg_type = RTM_GETLINK;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
  req.hdr.nlmsg_seq = ++nl_seq;
  req.hdr.nlmsg_pid = nlcb_pid;
  req.ifinfo.ifi_family = AF_UNSPEC;
  req.ifinfo.ifi_index = ifidx; /* Doesn't work... */
  req.ifinfo.ifi_flags = IFF_UP;
  req.ifinfo.ifi_change = 0xffffffff;

  send(nl_socket, &req, sizeof(req),0);
}


static void nl_handlemsg(struct nlmsghdr *msg, unsigned int len) {
  if (len < sizeof(*msg)) return;

  while(NLMSG_OK(msg,len)) {
    if (nlcb_run &&
	nlcb_pid == msg->nlmsg_pid &&
	nlcb_seq == msg->nlmsg_seq) {
      nlcb_function(msg, nlcb_args);

      if (msg->nlmsg_type == NLMSG_DONE ||
	  msg->nlmsg_type == NLMSG_ERROR) nlcb_run = 0;
    }

    if (NLMSG_PAYLOAD(msg, msg->nlmsg_len) >= sizeof(struct ifinfomsg)) {
      struct ifinfomsg *ifinfo = NLMSG_DATA(msg);

      switch(msg->nlmsg_type) {
      case RTM_NEWLINK:
	nl_callback(ifinfo->ifi_index,
		    (ifinfo->ifi_flags & IFF_UP)?(ifinfo->ifi_flags & IFF_RUNNING)?
		    OS_UP:OS_DOWN:OS_SHUT);
	break;
      case RTM_DELLINK:
	nl_callback(ifinfo->ifi_index, OS_REMOVED);
	break;
      default:
	break;
      }
    }
    msg = NLMSG_NEXT(msg,len);
  }
}


void nl_handleifmulticast() {
  char c[NLMSG_RECVSIZE];
  struct nlmsghdr *msg = (struct nlmsghdr *)c;
  int n;

  assert(nl_socket != -1);
  n = recv(nl_socket, c, NLMSG_RECVSIZE, 0);
  nl_handlemsg(msg, n);
}


static void nl_sync_dump() {
  char c[NLMSG_RECVSIZE];
  struct nlmsghdr *msg = (struct nlmsghdr *)c;
  int n;

  nlcb_seq = nl_seq;
  for(nlcb_run = 1; nlcb_run;) {
    n = recv(nl_socket, c, NLMSG_RECVSIZE, 0);
    assert(n >= 0);
    nl_handlemsg(msg,n);
  }
}


static void copy_ifdata(struct nlmsghdr *msg, void **args) {
  struct dhcp_interface *dhcpif = args[0];
  struct ifinfomsg *ifinfo = NLMSG_DATA(msg);
  struct rtattr *rta = IFLA_RTA(ifinfo);
  int len = NLMSG_PAYLOAD(msg, sizeof(*ifinfo));
  int found = 0;

  if (msg->nlmsg_type != RTM_NEWLINK) return;
  if (dhcpif->ifidx) return;

  for(; RTA_OK(rta, len); rta = RTA_NEXT(rta, len)) {
    switch(rta->rta_type) {
    case IFLA_IFNAME:
      if (!strncmp(dhcpif->ifname, (char *)RTA_DATA(rta), RTA_PAYLOAD(rta))) {
	dhcpif->ifidx = ifinfo->ifi_index;
	found |= 1;
      }
      break;
    case IFLA_ADDRESS:
      if (RTA_PAYLOAD(rta) == 6) { /* We can only handle ethernet like devices with 6 octet MAC */
	memcpy(dhcpif->ifhwaddr, RTA_DATA(rta), 6);
	found |= 2;
      }
      break;
    }
  }
  if (found == 3) nlcb_args[1] = (void *)1;
}


int nl_getifdata(const char *ifname, struct dhcp_interface *dhcpif) {
  struct {
    struct nlmsghdr hdr;
    struct ifinfomsg ifinfo;
  } req;

  strncpy(dhcpif->ifname, ifname, IFNAMSIZ);

  req.hdr.nlmsg_len = sizeof(req);
  req.hdr.nlmsg_type = RTM_GETLINK;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ROOT;
  req.hdr.nlmsg_seq = ++nl_seq;
  req.hdr.nlmsg_pid = nlcb_pid;
  req.ifinfo.ifi_family = AF_UNSPEC;

  if (send(nl_socket, &req, sizeof(req), 0) != sizeof(req)) return -1;

  nlcb_function = copy_ifdata;
  nlcb_args[0] = dhcpif;
  nlcb_args[1] = NULL;

  nl_sync_dump();

  return nlcb_args[1]?0:-1;
}


static int nl_inet_mask_len(const struct in_addr addr) {
  int rc = ffs(ntohl(addr.s_addr));
  return rc?33-rc:0;
}


static void checkaddrs(struct nlmsghdr *msg, void **args) {
  const struct dhcp_interface *dhcpif = args[0];
  const struct dhcp_config *current_config = args[1];
  struct ifaddrmsg *ifaddr = NLMSG_DATA(msg);
  struct rtattr *rta = IFA_RTA(ifaddr);
  int len = NLMSG_PAYLOAD(msg, sizeof(*ifaddr));
  struct in_addr address, broadcast;
  int cnt = 0;

  if (msg->nlmsg_type != RTM_NEWADDR) return;
  if (ifaddr->ifa_index != dhcpif->ifidx) return;

  if (nl_inet_mask_len(current_config->netmask) != ifaddr->ifa_prefixlen ||
      ifaddr->ifa_scope != RT_SCOPE_UNIVERSE)
    return; /* prefix/scope different - no need to scan attributes */

  for(; RTA_OK(rta, len); rta = RTA_NEXT(rta, len)) {
    switch(rta->rta_type) {
    case IFA_LOCAL:
      ++cnt;
      address.s_addr = *(u_int32_t *)RTA_DATA(rta);
      break;
    case IFA_BROADCAST:
      ++cnt;
      broadcast.s_addr = *(u_int32_t *)RTA_DATA(rta);
      break;
    }
  }

  if (cnt != 2) return;

  if (address.s_addr != current_config->address.s_addr ||
      broadcast.s_addr != current_config->broadcast.s_addr) return;

  args[2] = (void *)1; /* found a matching address */
}

static void generic_nlerror(struct nlmsghdr *msg, void **args) {
  struct nlmsgerr *err = NLMSG_DATA(msg);
  if (msg->nlmsg_type == NLMSG_ERROR && err->error) {
    args[0] = NULL;
    args[1] = (void *)err->error;
  }
}


void nl_configureif(const struct dhcp_interface *dhcpif){
  struct dhcp_config current_config = dhcpif->current_config;
  struct {
    struct nlmsghdr hdr;
    struct ifaddrmsg ifaddr;
    char opts[128];
  } req;

  /* fixup broadcast address if server doesn't supply it */
  if (!current_config.broadcast.s_addr)
    current_config.broadcast.s_addr =
      current_config.address.s_addr | ~current_config.netmask.s_addr;

  req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  req.hdr.nlmsg_type = RTM_GETADDR;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.hdr.nlmsg_seq = ++nl_seq;
  req.hdr.nlmsg_pid = nlcb_pid;

  req.ifaddr.ifa_family = AF_INET;
  req.ifaddr.ifa_index = dhcpif->ifidx; /* Maybe this will work in distant future */

  nlcb_function = checkaddrs;
  nlcb_args[0] = (void *)dhcpif;
  nlcb_args[1] = &current_config;
  nlcb_args[2] = NULL;

  send(nl_socket, &req, NLMSG_LENGTH(sizeof(struct ifaddrmsg)), 0); /* FIXME: error check? */
  nl_sync_dump();

  if (!nlcb_args[2]) { // no matching addr found, go & configure */
    struct rtattr *rta;

    nl_flushif(dhcpif);

    req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.hdr.nlmsg_type = RTM_NEWADDR;
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    req.hdr.nlmsg_seq = ++nl_seq;
    req.hdr.nlmsg_pid = nlcb_pid;

    req.ifaddr.ifa_family = AF_INET;
    req.ifaddr.ifa_prefixlen = nl_inet_mask_len(current_config.netmask);
    req.ifaddr.ifa_flags = 0;
    req.ifaddr.ifa_scope = RT_SCOPE_UNIVERSE;
    req.ifaddr.ifa_index = dhcpif->ifidx;

    rta = (struct rtattr *)((char *)&req + NLMSG_ALIGN(req.hdr.nlmsg_len));
    rta->rta_type = IFA_LOCAL;
    rta->rta_len = RTA_LENGTH(sizeof(u_int32_t));
    *((u_int32_t *)RTA_DATA(rta)) = current_config.address.s_addr;
    req.hdr.nlmsg_len = NLMSG_ALIGN(req.hdr.nlmsg_len) + RTA_LENGTH(sizeof(u_int32_t));

    rta = (struct rtattr *)((char *)&req + NLMSG_ALIGN(req.hdr.nlmsg_len));
    rta->rta_type = IFA_BROADCAST;
    rta->rta_len = RTA_LENGTH(sizeof(u_int32_t));
    *((u_int32_t *)RTA_DATA(rta)) = current_config.broadcast.s_addr;
    req.hdr.nlmsg_len = NLMSG_ALIGN(req.hdr.nlmsg_len) + RTA_LENGTH(sizeof(u_int32_t));

    send(nl_socket, &req, req.hdr.nlmsg_len, 0);
    nlcb_function = generic_nlerror;
    nl_sync_dump();
  }
}


void nl_flushif(const struct dhcp_interface *dhcpif) {
  nlcb_function = generic_nlerror;
  nlcb_args[0] = (void *)1;

  /* Take it easy: flush next address for interface until
     we receive an error */
  while(nlcb_args[0]) {
    struct {
      struct nlmsghdr hdr;
      struct ifaddrmsg ifaddr;
    } req;

    req.hdr.nlmsg_len = sizeof(req);
    req.hdr.nlmsg_type = RTM_DELADDR;
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    req.hdr.nlmsg_seq = ++nl_seq;
    req.hdr.nlmsg_pid = nlcb_pid;

    req.ifaddr.ifa_family = AF_INET;
    req.ifaddr.ifa_index = dhcpif->ifidx;

    send(nl_socket, &req, sizeof(req), 0);
    nl_sync_dump();
  }
}


void nl_configuregw(const struct dhcp_interface *dhcpif) {
  struct {
    struct nlmsghdr hdr;
    struct rtmsg rt;
    char opts[128];
  } req;
  struct rtattr *rta;
  u_int32_t defdst = 0;

  req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
  req.hdr.nlmsg_type = RTM_NEWROUTE;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_REPLACE;
  req.hdr.nlmsg_seq = ++nl_seq;
  req.hdr.nlmsg_pid = nlcb_pid;
  
  req.rt.rtm_family = AF_INET;
  req.rt.rtm_dst_len = 0;
  req.rt.rtm_src_len = 0;
  req.rt.rtm_tos = 0;
  req.rt.rtm_table = RT_TABLE_MAIN;
  req.rt.rtm_protocol = RTPROT_BOOT;
  req.rt.rtm_scope = RT_SCOPE_UNIVERSE;
  req.rt.rtm_type = RTN_UNICAST;
  req.rt.rtm_flags = 0;

  /* Some braindead internet providers send a gateway address
     that is outside the local subnet. Kernel needs to be
     explicitely told to use this route on the interface
     specified by RTA_OID below */
  if ((dhcpif->current_config.gateway.s_addr & dhcpif->current_config.netmask.s_addr) !=
      (dhcpif->current_config.address.s_addr & dhcpif->current_config.netmask.s_addr))
    req.rt.rtm_flags = RTNH_F_ONLINK;

  rta = (struct rtattr *)((char *)&req + NLMSG_ALIGN(req.hdr.nlmsg_len));
  rta->rta_type = RTA_DST;
  rta->rta_len = RTA_LENGTH(sizeof(u_int32_t));
  *((u_int32_t *)RTA_DATA(rta)) = defdst;
  req.hdr.nlmsg_len = NLMSG_ALIGN(req.hdr.nlmsg_len) + RTA_LENGTH(sizeof(u_int32_t));

  rta = (struct rtattr *)((char *)&req + NLMSG_ALIGN(req.hdr.nlmsg_len));
  rta->rta_type = RTA_GATEWAY;
  rta->rta_len = RTA_LENGTH(sizeof(u_int32_t));
  *((u_int32_t *)RTA_DATA(rta)) = dhcpif->current_config.gateway.s_addr;
  req.hdr.nlmsg_len = NLMSG_ALIGN(req.hdr.nlmsg_len) + RTA_LENGTH(sizeof(u_int32_t));

  rta = (struct rtattr *)((char *)&req + NLMSG_ALIGN(req.hdr.nlmsg_len));
  rta->rta_type = RTA_OIF;
  rta->rta_len = RTA_LENGTH(sizeof(u_int32_t));
  *((u_int32_t *)RTA_DATA(rta)) = dhcpif->ifidx;
  req.hdr.nlmsg_len = NLMSG_ALIGN(req.hdr.nlmsg_len) + RTA_LENGTH(sizeof(u_int32_t));

  nlcb_args[1] = 0;
  send(nl_socket, &req, req.hdr.nlmsg_len, 0);
  nl_sync_dump();

  if ((int)nlcb_args[1]) printf("error : %d\n", (int)nlcb_args[1]);
}
