
# load externals variables
include $(top_srcdir)/lib/libs.mk

# program to build
bin_PROGRAMS = rc-unitd

# program's sources
rc_unitd_SOURCES = main.cpp Common.h App.h App.cpp Config.h Config.cpp \
				   Device.h DeviceManager.h DeviceManager.cpp \
				   Joystick.h Joystick.cpp \
				   JoystickIgnore.h JoystickIgnore.cpp \
				   JoystickRemapping.h JoystickRemapping.cpp \
				   GameController.h GameController.cpp

# include paths
AM_CPPFLAGS = $(SDL_CFLAGS) \
			  $(LO_CFLAGS) $(LOPACK_INCLUDE) \
			  $(TINYXML2_CFLAGS) $(TINYOBJECT_INCLUDE) \
			  $(TCLAP_INCLUDE)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) \
			 $(LO_LIBS) $(LOPACK_LIBS) \
			 $(TINYXML2_LIBS) $(TINYOBJECT_LIBS)
