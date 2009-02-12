
# program to build
bin_PROGRAMS = rc-unitd

# program's sources
rc_unitd_SOURCES = main.cpp

# local library needed to build
rc_unitd_LDADD = -L../lib -lunit

# include paths
AM_CPPFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src/lib $(DBUS_CFLAGS) $(GLIB2_CFLAGS) $(LIBLO_CFLAGS)

# libs to link
AM_LDFLAGS = $(DBUS_LIBS) $(GLIB2_LIBS) $(LIBLO_LIBS)

if ENABLE_CWIID
AM_LDFLAGS += $(CWIID_LIBS)
endif

