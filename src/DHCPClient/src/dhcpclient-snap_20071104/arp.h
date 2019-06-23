#ifndef DHCP_ARP_H
#define DHCP_ARP_H

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

/* Send ARP query for IP address */

void ar_send_query_ip(int sock, int from_if, 
		      const struct in_addr my_ip, const unsigned char *my_mac,
		      const struct in_addr ip, const unsigned char *mac);

/* Parse reply */

int ar_parse_reply(const char *dframe, unsigned int plen,
		   unsigned char *smac, struct in_addr *sip,
		   unsigned char *dmac, struct in_addr *dip);

#endif
