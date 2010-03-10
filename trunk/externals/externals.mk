# external libs variables

# directories to build (rel to top)
EXTERNALS_DIRS = oscframework xmlframework tclap

# includes (in srcdir)
EXTERNALS_CFLAGS = \
		-I$(top_srcdir)/externals/xmlframework/src \
		-I$(top_srcdir)/externals/oscframework/src \
		-I$(top_srcdir)/externals/

# libs to link (in builddir)
EXTERNALS_LIBS = \
      	-L$(top_builddir)/externals/xmlframework/src/xmlframework \
      	-L$(top_builddir)/externals/oscframework/src/oscframework 
