
# load library variables
include $(top_srcdir)/lib/libs.mk

# program to build
bin_PROGRAMS = joyosc

# program's sources
joyosc_SOURCES = main.cpp Common.h Common.cpp ../shared.h App.h App.cpp \
                 Device.h Device.cpp DeviceExclusion.h DeviceExclusion.cpp \
                 DeviceManager.h DeviceManager.cpp Event.h \
                 Joystick.h Joystick.cpp \
                 JoystickIgnore.h JoystickIgnore.cpp \
                 JoystickRemapping.h JoystickRemapping.cpp \
                 GameController.h GameController.cpp \
                 GameControllerIgnore.h GameControllerIgnore.cpp \
                 GameControllerRemapping.h GameControllerRemapping.cpp

# include paths
AM_CXXFLAGS = $(SDL_CFLAGS) $(LO_CFLAGS) $(TINYXML2_CFLAGS) $(HELPERS_INCLUDE)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS) $(TINYXML2_LIBS)
