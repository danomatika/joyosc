
# this directory
thisdir = $(srcdir)

# include files
dist_configfiles_DATA = example_config.xml 85-rc-unitd.rules
configfilesdir = $(docdir)

# need to process the udev config file to insert the proper bin path
#
# udev needs the absolute path to rc-unitd-notifer so "BINDIR" in the rules file
# is replaced by $(bindir), which means it should work regardless of what prefix 
# is used 
#
install-data-local:
	sed 's:BINDIR:$(bindir):' $(thisdir)/85-rc-unitd.rules.source > \
	85-rc-unitd.rules

uninstall-local:
	rm -f 85-rc-unitd.rules

# include pd files
dist_pd_readme_DATA = $(thisdir)/pd/README
pd_readmedir = $(docdir)/pd
dist_rcpatches_DATA = $(thisdir)/pd/unit-patches/joystick-example.pd \
					  $(thisdir)/pd/unit-patches/unit-joystick.pd \
				 	  $(thisdir)/pd/unit-patches/unit-joystick-gui.pd \
				 	  $(thisdir)/pd/unit-patches/unit-joystick-help.pd
#				 	  $(thisdir)/pd/unit-patches/unit-wiimote.pd \
#				 	  $(thisdir)/pd/unit-patches/unit-wiimote-help.pd
rcpatchesdir = $(docdir)/pd/unit-patches

