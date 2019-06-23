DESTDIR=
PREFIX=/usr/local
MANDIR=$(PREFIX)/man
MANEXT=8
PIDDIR=/var/run
CFLAGS=-pedantic -std=gnu99 -g -Os -W -Wall -DDHCPCLIENT_SCRIPT=\"$(PREFIX)/sbin/dhcpclient-script\" -DDHCPCLIENT_VERSION=\"$(VERSION)\" -DDHCPCLIENT_PIDDIR=\"$(PIDDIR)\"
LDFLAGS=-g

all: dhcpclient dhcpclient.$(MANEXT) dhcpclient-script.sample dhcpclient.spec

DATE = 04 Nov 2007
VERSION = snap_20071104

OBJS = dhcpprotocol.o dhcpmessage.o netlink.o main.o \
	callscript.o sockets.o arp.o

dhcpclient: $(OBJS)
	$(CC) $(LDFLAGS) -o dhcpclient $(OBJS)

dhcpclient.spec: dhcpclient.spec.in
	sed -e "s#@VERSION@#$(VERSION)#g" <$< >$@

dhcpclient.$(MANEXT): dhcpclient.man.in
	sed -e "s#@PREFIX@#$(PREFIX)#g" -e "s#@MANEXT@#$(MANEXT)#g" -e "s#@VERSION@#$(VERSION)#g" -e "s#@DATE@#$(DATE)#g" <$< >$@

dhcpclient-script.sample: dhcpclient-script.sample.in
	sed -e "s#@PREFIX@#$(PREFIX)#g" <$< >$@

.PHONY: install
install: dhcpclient dhcpclient.$(MANEXT) dhcpclient-script.sample
	install -m 755 -t $(DESTDIR)$(PREFIX)/sbin dhcpclient
	install -m 755 -t $(DESTDIR)$(PREFIX)/sbin dhcpclient-script.sample
	install -m 644 -t $(DESTDIR)$(MANDIR)/man$(MANEXT) dhcpclient.$(MANEXT)

.depend: Makefile
	touch .depend
	makedepend -f .depend *.c

.PHONY: depend
depend:
	touch .depend
	makedepend -f .depend *.c

.PHONY: clean
clean:
	rm -f .depend* *.o dhcpclient dhcpclient.$(MANEXT) dhcpclient-script.sample dhcpclient.spec

.PHONY: dist
dist:
	rm -rf /tmp/dhcpclient-$(VERSION)
	mkdir /tmp/dhcpclient-$(VERSION)
	cp -p Makefile *.h *.c COPYING README CHANGES MANUAL dhcpclient.man.in dhcpclient-script.sample.in dhcpclient.spec.in /tmp/dhcpclient-$(VERSION)
	tar -cvjf /tmp/dhcpclient-$(VERSION).tar.bz2 -C /tmp dhcpclient-$(VERSION)

include .depend

