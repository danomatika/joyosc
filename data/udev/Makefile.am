
# this directory
thisdir = $(srcdir)

# include files
dist_configfiles_DATA = 85-rc-unitd.rules
configfilesdir = $(docdir)

# need to process the udev config file to insert the proper bin path
#
# udev needs the absolute path to rc-unitd-notifer so "BINDIR" in the rules file
# is replaced by $(bindir), which means it should work regardless of what prefix 
# is used 
#
all-local:
	sed 's:BINDIR:$(bindir):' $(thisdir)/85-rc-unitd.rules.source > 85-rc-unitd.rules

clean-local:
	rm -f 85-rc-unitd.rules
