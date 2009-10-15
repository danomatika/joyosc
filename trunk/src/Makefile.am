
# go into these dirs and process makefiles
SUBDIRS = lib lsjs rc-unitd rc-unit-notifier

# data files install paths
# from http://firefighter.cs.binghamton.edu/cgi-bin/viewvc.cgi/releases/freeloader_0.7.3/FUSE/fuse/util/Makefile.am?limit_changes=0&view=markup&root=svn&pathrev=6

# conditional so distcheck will not try to install configs b/c it dosent
# like putting configs in static places aka /etc (mabey im just lazy)
if ENABLE_CONFIG

# data files to install and where, automatically copied or removed on (un)install
dist_config_DATA = unitd.conf joy_names.conf
configdir = $(DESTDIR)/etc/rc-unitd
dist_dbus_DATA = rc-unitd.conf
dbusdir = $(DESTDIR)/etc/dbus-1/system.d

# need to process this, so cant have it automatically handled
EXTRA_DIST = 85-rc-unitd.rules
udevdir = $(DESTDIR)/etc/udev/rules.d

# need to process the udev config file to insert the proper bin path
#
# udev needs the absolute path to rc-unitd-notifer so "BINDIR" in the rules file
# is replaced by $(bindir), which means it should work regardless of what prefix 
# is used 
#
# setup to use sysconfdir and copies processes file to tmpdir so make distcheck
# aka readonly vpath build works
tmpdir = /tmp
install-data-local:
	sed 's:BINDIR:$(bindir):' 85-rc-unitd.rules > $(tmpdir)/85-rc-unitd.rules
	$(INSTALL_DATA) $(tmpdir)/85-rc-unitd.rules $(udevdir)/85-rc-unitd.rules
	rm -f $(tmpdir)/85-rc-unitd.rules

# and make sure we remove the config dir and rules file
uninstall-local:
	rmdir $(configdir)
	rm -f $(udevdir)/85-rc-unitd.rules

endif
