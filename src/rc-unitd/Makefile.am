
# load externals variables
include $(top_srcdir)/externals/externals.mk

# program to build
bin_PROGRAMS = rc-unitd

# program's sources
rc_unitd_SOURCES = main.cpp App.h App.cpp

# local libraries needed to build (builddir)
rc_unitd_LDADD = -L$(top_builddir)/src/lib -lunit \
				 $(EXTERNALS_LIBS) -loscframework -lxmlframework

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_CFLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS) \
			  -I$(top_srcdir)/src/lib

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS)

#if ENABLE_CWIID
#AM_LDFLAGS += $(CWIID_LIBS)
#endif

