# unitd lib of classes

# the library to build, dont install just link to execs
noinst_LIBRARIES = libunit.a

# libs sources, headers here because we dont want to install them
libunit_a_SOURCES =	Config.h Config.cpp \
							DBus.h DBus.cpp \
							Devices.h Devices.cpp \
							Joystick_Device.h Joystick_Device.cpp \
							Osc_Server.h Osc_Server.cpp \
							Serial_Device.h Serial_Device.cpp

# optionally build with wiimote classes
if ENABLE_CWIID
libunit_a_SOURCES += Wiimote.h Wiimote.cpp
endif

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(DBUS_CFLAGS) $(GLIB2_CFLAGS) $(LIBLO_CFLAGS)

# libs to link
AM_LDFLAGS = $(DBUS_LIBS) $(GLIB2_LIBS) $(LIBLO_LIBS)

# optionally link with cwiid
if ENABLE_CWIID
AM_LDFLAGS += $(CWIID_LIBS)
endif

