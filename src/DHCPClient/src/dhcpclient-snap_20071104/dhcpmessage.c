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

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>

#include "dhcpmessage.h"



static const unsigned char vendcookie[] = { 99, 130, 83, 99 };


/* taken from iputils which basically copied it from RFC 1071 */
static u_short
in_cksum(const u_short *addr, register int len, u_short csum)
{
        int nleft = len;
        const u_short *w = addr;
        u_short answer;
        int sum = csum;

        /*
         *  Our algorithm is simple, using a 32 bit accumulator (sum),
         *  we add sequential 16 bit words to it, and at the end, fold
         *  back all the carry bits from the top 16 bits into the lower
         *  16 bits.
         */
        while (nleft > 1)  {
                sum += *w++;
                nleft -= 2;
        }

        /* mop up an odd byte, if necessary */
        if (nleft == 1)
                sum += htons(*(u_char *)w << 8);

        /*
         * add back carry outs from top 16 bits to low 16 bits
         */
        sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
        sum += (sum >> 16);                     /* add carry */
        answer = ~sum;                          /* truncate to 16 bits */
        return (answer);
}



void dm_init(struct dhcp_message *msg) {
  memset(msg, 0, sizeof(*msg));
  msg->pos = msg->options+4;
  // msg->flags = htons(0x8000);
  memcpy(msg->options, vendcookie, 4);
}


void dm_add_option(struct dhcp_message *msg, u_int8_t option,
		   u_int8_t length, void *opt) {
  u_int8_t *pos = msg->pos;

  if (&msg->options[MAX_OPT_LEN] - pos < length + 2) abort();

  *pos++ = option;
  *pos++ = length;
  memcpy(pos, opt, length);
  pos += length;

  msg->pos = pos;
}


void dm_finish_options(struct dhcp_message *msg) {
  if (msg->pos == &msg->options[MAX_OPT_LEN]) abort();

  *msg->pos++ = 255;
}


u_int8_t *dm_next_option(struct dhcp_message *msg) {
  u_int8_t *pos = msg->pos;
  u_int8_t length;

  /* End of packet */
  if (pos >= msg->last) return NULL;

  /* skip pad packets */
  while(!*pos) if (++pos >= msg->last) return NULL;

  /* End of option marker */
  while (*pos == 255) {
    /* Overload option handling */
    if (msg->currentblock < msg->overload) { // currentblock: 0,1,3
      msg->currentblock++;
      if (msg->overload & DHCP_OVERLOAD_FILE & msg->currentblock) {
	pos = &msg->file[0];
        msg->last = &msg->file[128];
      } else { // SNAME or BOTH
	pos = &msg->sname[0];
        msg->last = &msg->sname[64];
        msg->currentblock = DHCP_OVERLOAD_BOTH; // definitely last block
      }
      /* skip pad packets */
      while(!*pos) if (++pos >= msg->last) return NULL;
    } else {
      return NULL;
    }
  }

  /* Actually, this is extra paranoia. Even if pos+1
   * leaves the dhcp_message structure, the next
   * check would catch this as long as we don't
   * try to access an unmapped page ;-)
   */   
  if (pos+1 >= msg->last) return NULL;
  
  length = *(pos+1);
  /* Length overflow */
  if (pos + length + 2 > msg->last) return NULL;

  msg->pos = pos + length+2;

  return pos;
}


int dm_send_msg(int sock, struct in_addr *to,
		struct dhcp_message *msg) {
  struct sockaddr_in toadr;
  int length = msg->pos - &msg->op;
  
  toadr.sin_family = AF_INET;
  toadr.sin_port = htons(BOOTP_SERVER_PORT);
  toadr.sin_addr = *to;
  return sendto(sock, &msg->op, length, 0,
		(const struct sockaddr *)&toadr, sizeof(toadr));
}


int dm_parse_msg_raw(char *dframe, int plen,
		     struct in_addr *from_ip, struct dhcp_message *msg) {
  struct iphdr *ip;
  struct udphdr *udp;
  int iphlen, udplen;
  u_short checksum;

  if (plen < sizeof(*ip)) return -1;

  /* Verify IP: IP, UDP, ... */
  ip = (struct iphdr *)dframe;
  iphlen = 4 * ip->ihl;
  if (ip->version != 4) return -1; /* no ipv4 packet */
  if (plen < iphlen || iphlen < 20) return -1; /* ip header too short */
  if (plen < ntohs(ip->tot_len)) return -1; /* packet too short */
  if (in_cksum((u_short *)ip, iphlen, 0)) return -1; /* checksum wrong */
  if (ip->protocol != IPPROTO_UDP) return -1; /* no udp */
  if (ip->frag_off & htons(IP_OFFMASK)) return -1; /* won't parse fragments */

  from_ip->s_addr = ip->saddr;

  /* UDP src, destination */
  udp = (struct udphdr *)&dframe[iphlen];
  if (udp->source != htons(BOOTP_SERVER_PORT)) return -1;
  if (udp->dest != htons(BOOTP_CLIENT_PORT)) return -1;

  udplen = ntohs(udp->len);
  if (iphlen + udplen > plen) return -1; /* truncated BOOTPREPLY */

  /* check udp checksum */
  if (udp->check != 0 && udp->check != 0xffff) {
    /* FIXME: checksum 0xffff has to be treated as 0. Until I've constructed
       a testcase, treat 0xffff as no checksum */
    /* RFC 768: Calculate the checksum including the pseudo header
       s-ip(4), d-ip(4), 0x00(1), proto(1), udp-length(2) */
    checksum = htons(IPPROTO_UDP);
    checksum = in_cksum((u_short *)&udp->len, 2, checksum);
    checksum = in_cksum((u_short *)&ip->saddr, 8, ~checksum); // ip options might follow
    checksum = in_cksum((u_short *)udp, udplen, ~checksum); // saddr + daddr + udp
    if (checksum) return -1; /* udp packet checksum wrong */
  }
  udplen -= sizeof(*udp); /* udplen is now dhcplen! */
  if (udplen < &msg->options[4] - &msg->op) return -1; /* BOOTPREPLY too short */

  memcpy(&msg->op, &dframe[iphlen+sizeof(*udp)], udplen);

  if (memcmp(msg->options, vendcookie, 4)) return -1; /* No DHCP message */

  msg->pos = msg->options+4;
  msg->last = msg->pos + (udplen + &msg->options[0] - &msg->op);
  msg->overload = DHCP_OVERLOAD_NONE;
  msg->currentblock = DHCP_OVERLOAD_NONE;

  return 0;
}


int dm_broadcast_msg_raw(int sock, struct in_addr from_ip,
			 int from_if, struct dhcp_message *msg) {
  int length =  msg->pos - &msg->op;
  struct {
    struct iphdr ip;
    struct udphdr udp;
  } hdr;
  struct sockaddr_ll addr = {
    .sll_family = AF_PACKET,
    .sll_protocol = htons(ETH_P_IP),
    .sll_ifindex = from_if,
    .sll_hatype = 0,
    .sll_pkttype = 0,
    .sll_halen = ETH_ALEN
  };
  struct iovec iov[] = {
    { .iov_base = &hdr, .iov_len = sizeof(hdr) },
    { .iov_base = &msg->op, .iov_len = length }
  };
  struct msghdr msghdr = {
    .msg_name = &addr,
    .msg_namelen = sizeof(addr),
    .msg_iov = iov,
    .msg_iovlen = 2,
    .msg_control = NULL,
    .msg_controllen = 0,
    .msg_flags = 0
  };
  u_short checksum;

  memset(addr.sll_addr, 255, ETH_ALEN); /* broadcast */

  hdr.ip.version = 4;
  hdr.ip.ihl = 5; /* minimal 20 byte header w/o options */
  hdr.ip.tos = 0;
  hdr.ip.tot_len = htons(length + sizeof(hdr));
  hdr.ip.id = 0;
  hdr.ip.frag_off = htons(IP_DF);
  hdr.ip.ttl = 64;
  hdr.ip.protocol = IPPROTO_UDP;
  hdr.ip.saddr = from_ip.s_addr;
  hdr.ip.daddr = 0xffffffff;
  hdr.ip.check = 0;
  hdr.ip.check = in_cksum((u_short *)&hdr.ip, 20, hdr.ip.check);

  hdr.udp.source = htons(BOOTP_CLIENT_PORT);
  hdr.udp.dest = htons(BOOTP_SERVER_PORT);
  hdr.udp.len = htons(length + sizeof(struct udphdr));
  hdr.udp.check = 0; // set to 0 for calculation
  /* RFC 768: Calculate the checksum including the pseudo header
     s-ip(4), d-ip(4), 0(1), proto(1), udp-length(2) */
  checksum = htons(IPPROTO_UDP);
  checksum = in_cksum((u_short *)&hdr.udp.len, 2, checksum);
  checksum = in_cksum((u_short *)&hdr.ip.saddr, 16, ~checksum); // saddr, daddr, udp-header
  checksum = in_cksum((u_short *)&msg->op, length, ~checksum);
  hdr.udp.check = checksum?checksum:0xffff;

  return sendmsg(sock, &msghdr, 0);
}

