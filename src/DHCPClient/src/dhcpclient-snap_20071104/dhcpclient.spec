#
# spec file for package dhcpclient (Version snap_20071104)
#
# Copyright (c) 2006, 2007 Joerg Mayer
#
# Copied and modified from dhcpcd.spec, which is
#
# Copyright (c) 2006 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
#

# norootforbuild
# usedforbuild    aaa_base acl attr audit-libs autoconf automake bash bind-libs bind-utils binutils bison bzip2 coreutils cpio cpp cracklib cvs cyrus-sasl db diffutils e2fsprogs file filesystem fillup findutils flex gawk gcc gdbm gdbm-devel gettext gettext-devel glibc glibc-devel glibc-locale gpm grep groff gzip info insserv klogd less libacl libattr libcom_err libgcc libnscd libstdc++ libtool libxcrypt libzio m4 make man mktemp module-init-tools ncurses ncurses-devel net-tools netcfg openldap2-client openssl pam pam-modules patch perl permissions popt procinfo procps psmisc pwdutils rcs readline rpm sed strace sysvinit tar tcpd texinfo timezone unzip util-linux vim zlib zlib-devel

Name:           dhcpclient
Version:        snap_20071104
Release:        1
Summary:        A small and modular dhcp client
URL:            http://www.flamewarmaster.de/software/dhcpclient/
License:        GPL
Group:          Productivity/Networking/Boot/Clients
Provides:       dhcp_client
Requires:       net-tools
Autoreqprov:    on
Source:         http://www.flamewarmaster.de/software/dhcpclient/dhcpclient-%{version}.tar.bz2
# Adapt dhcpclient-script to SUSE
#Patch1:         dhcpclient_snap_20071104-script.diff
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
dhcpclient gets an IP address and other information from a corresponding
DHCP server, configures the network interface automatically, and tries
to renew the lease time depending on the command line option. It will
immediately detect when a link goes down or up and react accordingly.



Authors:
--------
    Stefan Rompf <sux@loplof.de>

%prep
%setup -n dhcpclient-%{version}

%build
make PREFIX= RPM_OPT_FLAGS="$RPM_OPT_FLAGS" LDFLAGS=-pie

%install
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;
mkdir -p $RPM_BUILD_ROOT/sbin
mkdir -p $RPM_BUILD_ROOT/%{localstatedir}
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man8
make DESTDIR=$RPM_BUILD_ROOT PREFIX= MANDIR=%{_mandir} install
#cp -p $RPM_SOURCE_DIR/README.SuSE.dhcpclient README.SuSE

%postun
%{insserv_cleanup}

%files
%defattr(-,root,root)
%doc CHANGES COPYING MANUAL README
%doc %{_mandir}/man8/dhcpclient.8*
/sbin/dhcpclient
/sbin/dhcpclient-script.sample

%clean
rm -rf $RPM_BUILD_ROOT

%changelog -n dhcpclient
* Mon Oct 09 2006 - stefan@loplof.de
- replaced Makefile patch
* Tue Apr 05 2006 - jmayer@loplof.de
- Create initial specfile
