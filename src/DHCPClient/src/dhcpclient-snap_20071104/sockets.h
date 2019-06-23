#ifndef DHCP_SOCKETS_H
#define DHCP_SOCKETS_H

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

struct dhcp_interface;

int sk_openudp(struct dhcp_interface *dhcpif);

int sk_openraw(struct dhcp_interface *dhcpif);


#endif
