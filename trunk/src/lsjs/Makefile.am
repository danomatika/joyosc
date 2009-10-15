
# program to build
bin_PROGRAMS = lsjs

# program's sources
lsjs_SOURCES = main.cpp

# local library needed to build
lsjs_LDADD = -L../lib -lunit

# include paths
AM_CPPFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src/lib $(LIBLO_CFLAGS)

# libs to link
AM_LDFLAGS = $(LIBLO_LIBS)

