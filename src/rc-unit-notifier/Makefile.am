
# load externals variables
include $(top_srcdir)/lib/libs.mk

# program to build
bin_PROGRAMS = rc-unit-notifier

# program's sources
rc_unit_notifier_SOURCES = main.cpp

# include paths
AM_CPPFLAGS = $(SDL_CFLAGS) $(LO_CFLAGS) $(LOPACK_INCLUDE) $(TCLAP_INCLUDE)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS) $(LOPACK_LIBS)
