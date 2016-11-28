
# load library variables
include $(top_srcdir)/lib/libs.mk

# program to build
bin_PROGRAMS = joyosc

# program's sources
joyosc_SOURCES = main.cpp Common.h App.h App.cpp Config.h Config.cpp \
				 Device.h DeviceManager.h DeviceManager.cpp \
				 Joystick.h Joystick.cpp \
				 JoystickIgnore.h JoystickIgnore.cpp \
				 JoystickRemapping.h JoystickRemapping.cpp \
				 GameController.h GameController.cpp \
				 GameControllerIgnore.h GameControllerIgnore.cpp \
				 GameControllerRemapping.h GameControllerRemapping.cpp

# include paths
AM_CXXFLAGS = $(SDL_CFLAGS) \
			  $(LO_CFLAGS) $(LOPACK_INCLUDE) \
			  $(TINYXML2_CFLAGS) $(TINYOBJECT_INCLUDE) \
			  $(HELPERS_INCLUDE)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) \
			 $(LO_LIBS) $(LOPACK_LIBS) \
			 $(TINYXML2_LIBS) $(TINYOBJECT_LIBS)
