# external libs variables

# directories to build (rel to top)
EXTERNALS_DIRS = oscframework xmlframework tclap

# includes (in srcdir)
EXTERNALS_CFLAGS = \
		-I$(top_srcdir)/externals/xmlframework \
		-I$(top_srcdir)/externals/oscframework \
		-I$(top_srcdir)/externals/

# libs to link (in builddir)
EXTERNALS_LIBS = \
      	-L$(top_builddir)/externals/xmlframework/ \
      	-L$(top_builddir)/externals/oscframework/ 
