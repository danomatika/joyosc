
# load externals variables
include $(top_srcdir)/externals/externals.mk

# program to build
bin_PROGRAMS = lsjs

# program's sources
lsjs_SOURCES = main.cpp

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_CFLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS)

# libs to link
AM_LDFLAGS = $(LDFLAGS) $(SDL_LIBS) $(LO_LIBS)
