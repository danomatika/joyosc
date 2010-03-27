
# dirs to delve into
SUBDIRS = src

# include pd files
dist_pd_DATA = $(top_srcdir)/pd/README
pddir = $(docdir)/pd
dist_rcpatches_DATA = $(top_srcdir)/pd/unit-patches/Example-pdrecv.pd \
					  $(top_srcdir)/pd/unit-patches/unit-joystick.pd \
				 	  $(top_srcdir)/pd/unit-patches/unit-joystick-help.pd \
				 	  $(top_srcdir)/pd/unit-patches/unit-wiimote.pd \
				 	  $(top_srcdir)/pd/unit-patches/unit-wiimote-help.pd
rcpatchesdir = $(docdir)/pd/unit-patches

# include the readme as well
dist_readme_DATA = $(top_srcdir)/README
readmedir = $(docdir)

# kludge to disable installation of udev rules so distcheck passes
DISTCHECK_CONFIGURE_FLAGS = --disable-config
