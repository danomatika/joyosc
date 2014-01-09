# unitd lib of classes

# load externals variables
include $(top_srcdir)/externals/externals.mk

# the library to build, dont install just link to execs
noinst_LIBRARIES = libunit.a

# libs sources, headers here because we dont want to install them
dist_libunit_a_SOURCES =	Common.h Log.h \
					Config.h Config.cpp \
					Device.h \
					JoystickDevice.h JoystickDevice.cpp \
					JoystickManager.h JoystickManager.cpp

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_FLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS)
