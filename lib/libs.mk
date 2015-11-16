# external library variables

# subdirs to delve into
LIB_SUBDIRS = 

# includes (in srcdir) & libs to link (local libs in builddir)
# set local paths if building locally

# lopack
if LOCAL_LOPACK
LIB_SUBDIRS += lopack
LOPACK_INCLUDE = -I$(top_srcdir)/lib/lopack/src
LOPACK_LIBS = -L$(top_builddir)/lib/lopack/src/lopack -llopack
else
LOPACK_INCLUDE =
LOPACK_LIBS = -llopack
endif

# tinyobject
if LOCAL_TINYOBJECT
LIB_SUBDIRS += tinyobject
TINYOBJECT_INCLUDE = -I$(top_srcdir)/lib/tinyobject/src
TINYOBJECT_LIBS = -L$(top_builddir)/lib/tinyobject/src/tinyobject -ltinyobject
else
TINYOBJECT_INCLUDE =
TINYOBJECT_LIBS = -ltinyobject
endif

# tclap
TCLAP_INCLUDE = -I$(top_srcdir)/lib/cpphelpers -I$(top_srcdir)/lib/cpphelpers/tclap
