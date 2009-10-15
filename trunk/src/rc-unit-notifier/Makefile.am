
# program to build
bin_PROGRAMS = rc-unit-notifier

# program's sources
rc_unit_notifier_SOURCES = main.cpp

# local library needed to build
rc_unit_notifier_LDADD = -L../lib -lunit

# include paths
AM_CPPFLAGS = $(CXXFLAGS) -I$(top_srcdir)/src/lib $(DBUS_CFLAGS) $(GLIB2_CFLAGS)

# libs to link
AM_LDFLAGS = $(DBUS_LIBS) $(GLIB2_LIBS)

