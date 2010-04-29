# xmlframework

# the library to build, dont install just link to execs
noinst_LIBRARIES = libxmlframework.a

# libs sources, headers here because we dont want to install them
libxmlframework_a_SOURCES = \
		Config.h Log.h \
        xmlframework.h \
        Xml.h Xml.cpp \
        XmlObject.h XmlObject.cpp \
        tinyxml/tinystr.h \
        tinyxml/tinystr.cpp \
        tinyxml/tinyxml.h \
        tinyxml/tinyxml.cpp \
        tinyxml/tinyxmlerror.cpp \
        tinyxml/tinyxmlparser.cpp

# include paths
AM_CPPFLAGS = $(CXXFLAGS)

# libs to link
AM_LDFLAGS =

