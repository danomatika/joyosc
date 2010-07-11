# oscframework

# the library to build, dont install just link to execs
noinst_LIBRARIES = liboscframework.a

# libs sources, headers here because we dont want to install them
dist_liboscframework_a_SOURCES = \
		Log.h Exception.h \
		oscframework.h \
		OscReceiver.h OscReceiver.cpp \
		OscObject.h OscObject.cpp \
		OscSender.h OscSender.cpp \
		OscTypes.h OscTypes.cpp

# include paths
AM_CPPFLAGS = $(CXXFLAGS) $(LO_CFLAGS)

# libs to link
AM_LDFLAGS = $(LO_LIBS)

