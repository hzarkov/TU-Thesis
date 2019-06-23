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


#include <netinet/if_ether.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netpacket/packet.h>

#include "arp.h"

void ar_send_query_ip(int sock, int from_if,
		      const struct in_addr my_ip, const unsigned char *my_mac,
		      const struct in_addr ip, const unsigned char *mac) {
  struct ether_arp hdr = {
    .ea_hdr = {
      .ar_hrd = htons(ARPHRD_ETHER),
      .ar_pro = htons(ETH_P_IP),
      .ar_hln = ETH_ALEN,
      .ar_pln = 4,
      .ar_op = htons(ARPOP_REQUEST)
    }
  };
  struct sockaddr_ll addr = {
    .sll_family = AF_PACKET,
    .sll_protocol = htons(ETH_P_ARP),
    .sll_ifindex = from_if,
    .sll_hatype = 0,
    .sll_pkttype = 0,
    .sll_halen = ETH_ALEN
  };

  if (mac)
    memcpy(addr.sll_addr, mac, ETH_ALEN);
  else
    memset(addr.sll_addr, 0xff, ETH_ALEN);

  memcpy(hdr.arp_sha, my_mac, ETH_ALEN);
  memcpy(hdr.arp_spa, &my_ip.s_addr, 4);
  memset(hdr.arp_tha, 0, ETH_ALEN);
  memcpy(hdr.arp_tpa, &ip.s_addr, 4);

  sendto(sock, &hdr, sizeof(hdr), 0, (const struct sockaddr *)&addr, sizeof(addr));
}


int ar_parse_reply(const char *dframe, unsigned int plen,
		   unsigned char *smac, struct in_addr *sip,
		   unsigned char *dmac, struct in_addr *dip) {
  const struct ether_arp *hdr = (const struct ether_arp *)dframe;

  if (plen < sizeof(*hdr)) return -1; /* can be larger - ethernet trailer */

  /* Not a ARP reply on ethernet */
  if (hdr->ea_hdr.ar_hrd != htons(ARPHRD_ETHER) ||
      hdr->ea_hdr.ar_pro != htons(ETH_P_IP) ||
      hdr->ea_hdr.ar_hln != ETH_ALEN ||
      hdr->ea_hdr.ar_pln != 4 ||
      hdr->ea_hdr.ar_op != htons(ARPOP_REPLY)) return -1;

  memcpy(smac, hdr->arp_sha, ETH_ALEN);
  memcpy(&sip->s_addr, hdr->arp_spa, 4);
  memcpy(dmac, hdr->arp_tha, ETH_ALEN);
  memcpy(&dip->s_addr, hdr->arp_tpa, 4);

  return 0;
}


