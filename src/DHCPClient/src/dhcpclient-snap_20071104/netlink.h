#ifndef DHCPCLIENT_NETLINK_H
#define DHCPCLIENT_NETLINK_H

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

extern int nl_socket;

int nl_open();

void nl_close();

/* query interface status to be asyncreceived on nl_socket */
void nl_queryifstatus();

/* get status for one interface if data on netlink is available,
   nl_callback is implemented outside netlink.c! */
typedef enum {
  OS_REMOVED, /* interface removed from system */
  OS_SHUT, /* shutdown */
  OS_DOWN, /* oper down */
  OS_UP /* oper up */
} nl_ostatus ;
void nl_callback(int ifidx, nl_ostatus status);

void nl_handleifmulticast();

/* Get index for name synchronously, expensive! */
struct dhcp_interface;
int nl_getifdata(const char *ifname, struct dhcp_interface *dhcpif);


/* flush interface address, kernel may implicitely clear routes */
void nl_flushif(const struct dhcp_interface *dhcpif);

/* configure interface to current_config, if needed */
void nl_configureif(const struct dhcp_interface *dhcpif);

/* configure default gateway to current_config, if needed */
void nl_configuregw(const struct dhcp_interface *dhcpif);

#endif

