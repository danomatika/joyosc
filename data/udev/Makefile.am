
# this directory
thisdir = $(srcdir)

# include files
dist_configfiles_DATA = 85-joyosc.rules.source
configfilesdir = $(docdir)

# need to process the udev config file to insert the proper bin path
#
# udev needs the absolute path to joyosc-notifer so "BINDIR" in the rules file
# is replaced by $(bindir), which means it should work regardless of what prefix 
# is used 
#
all-local:
	sed 's:BINDIR:$(bindir):' $(thisdir)/85-joyosc.rules.source > 85-joyosc.rules

clean-local:
	rm -f 85-joyosc.rules
