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

#include "sockets.h"
#include "dhcpprotocol.h"
#include "dhcpmessage.h"

#include <assert.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <linux/types.h>
#include <linux/filter.h>


int sk_openudp(struct dhcp_interface *dhcpif) {
  struct sockaddr_in addr;
  int sock;
  int i = 1;

  assert(dhcpif->sock_udp == -1);

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock == -1) return -1;

  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &i, sizeof(i)))
    goto err_close;

  if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, dhcpif->ifname, 1+strlen(dhcpif->ifname)))
    goto err_close;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = 0;
  addr.sin_port = htons(BOOTP_CLIENT_PORT);

  if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)))
    goto err_close;

  if (fcntl(sock, F_SETFD, FD_CLOEXEC))
    goto err_close;

  dhcpif->sock_udp = sock;

  return 0;

 err_close:
  close(sock);
  return -1;
}


/* I've selected not to see LL header, so BPF doesn't see it, too.
 * The filter may also pass non-IP and non-ARP packets, but we do
 * a more complete check when receiving the message in userspace
 */
static struct sock_filter insns[] = {
  /* check for udp */
  BPF_STMT(BPF_LD|BPF_B|BPF_ABS, 9),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, IPPROTO_UDP, 2, 0), /* L5, L1, is UDP? */
  /* ugly check for arp on ethernet-like and IPv4 */
  BPF_STMT(BPF_LD|BPF_W|BPF_ABS, 2), /* L1: */
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, 0x08000604, 3, 4), /* L3, L4 */
  /* skip IP header */
  BPF_STMT(BPF_LDX|BPF_B|BPF_MSH, 0), /* L5: */
  /* check udp source port */
  BPF_STMT(BPF_LD|BPF_H|BPF_IND, 0),
  BPF_JUMP(BPF_JMP|BPF_JEQ|BPF_K, BOOTP_SERVER_PORT, 0, 1), /* L3, L4 */
  /* returns */
  BPF_STMT(BPF_RET|BPF_K, ~0UL), // L3: pass
  BPF_STMT(BPF_RET|BPF_K, 0), // L4: reject
};
static struct sock_fprog filter = {
  sizeof insns / sizeof(insns[0]),
  insns
};


int sk_openraw(struct dhcp_interface *dhcpif) {
  struct sockaddr_ll addr = {
    .sll_family = PF_PACKET,
    .sll_protocol = 0,
    .sll_ifindex = dhcpif->ifidx
  };
  int sock;

  sock = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
  if (sock == -1) return -1;

  (void)setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter));

  if (bind(sock, (const struct sockaddr *)&addr, sizeof(addr)))
    goto err_close;

  if (fcntl(sock, F_SETFD, FD_CLOEXEC))
    goto err_close;

  dhcpif->sock_raw = sock;

  return 0;

 err_close:
  close(sock);
  return -1;
}

