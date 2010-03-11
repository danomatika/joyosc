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

# optionally build with wiimote classes
#if ENABLE_CWIID
#libunit_a_SOURCES += Wiimote.h Wiimote.cpp
#endif

# local libraries needed to build (builddir)
libunit_a_LIBADD = $(EXTERNALS_LIBS) -lxmlframework -loscframework

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(SDL_FLAGS) $(LO_CFLAGS) $(EXTERNALS_CFLAGS)
#		-I$(top_srcdir)/../externals/xmlframework/src \
#		-I$(top_srcdir)/../externals/oscframework/src \
#		-I$(top_srcddir)/externals/

#$(EXTERNALS_CFLAGS)

# libs to link
AM_LDFLAGS = $(SDL_LIBS) $(LO_LIBS)
#$(EXTERNALS_LIBS) -lxmlframework -loscframework
#			-L$(top_srcdir)/../externals/xmlframework/src/xmlframework \
#    	     -L$(top_srcdir)/../externals/oscframework/src/oscframework \
#		     -lxmlframework -loscframework
#$(EXTERNALS_LIBS)

# optionally link with cwiid
#if ENABLE_CWIID
#AM_LDFLAGS += $(CWIID_LIBS)
#endif

