# unitd lib of classes

# load externals variables
include $(top_srcdir)/externals/externals.mk

# the library to build, dont install just link to execs
noinst_LIBRARIES = libunit.a

# libs sources, headers here because we dont want to install them
dist_libunit_a_SOURCES = Config.cpp \
						 Joystick.cpp JoystickIgnore.cpp JoystickRemapping.cpp \
						 GameController.cpp \
						 DeviceManager.cpp

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_CFLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS)
