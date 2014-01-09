
# load externals variables
include $(top_srcdir)/externals/externals.mk

# program to build
bin_PROGRAMS = rc-unit-notifier

# program's sources
rc_unit_notifier_SOURCES = main.cpp

# local libraries needed to build (builddir)
rc_unit_notifier_LDADD = -L$(top_builddir)/src/lib -lunit \
						 $(EXTERNALS_LIBS) -loscframework -lxmlframework

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_CFLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS) 

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS)
