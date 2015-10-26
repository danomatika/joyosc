
# load externals variables
include $(top_srcdir)/externals/externals.mk

# program to build
bin_PROGRAMS = rc-unitd

# program's sources
rc_unitd_SOURCES = main.cpp Common.h App.h App.cpp Config.h Config.cpp \
				   Device.h DeviceManager.h DeviceManager.cpp \
				   Joystick.h Joystick.cpp \
				   JoystickIgnore.h JoystickIgnore.cpp \
				   JoystickRemapping.h JoystickRemapping.cpp \
				   GameController.h GameController.cpp

# local libraries needed to build (builddir)
rc_unitd_LDADD = $(EXTERNALS_LIBS) -loscframework -lxmlframework

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_CFLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS)

# libs to link
AM_LDFLAGS = $(LDFLAGS) $(SDL_LIBS) $(LO_LIBS)
