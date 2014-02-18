#########################################
##### Prelude #####

AC_INIT([rc-unitd], [0.3.5], [danomatika@gmail.com], [rc-unitd], [http://github.com/danomatika/rc-unitd])
AC_CONFIG_SRCDIR([src/rc-unitd/main.cpp])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CANONICAL_HOST

#########################################
##### Build Options #####

DEBUG_CXXFLAGS="-g -Wall -Werror -Wno-uninitialized -O0"
RELEASE_CXXFLAGS="-g -O2"

#########################################
##### Check for programs/libs #####

# check progs
AC_PROG_CXX
AC_PROG_RANLIB

# using c++ compiler and linker
AC_LANG_CPLUSPLUS

# check for headers
AC_HEADER_STDC
AC_CHECK_HEADERS(sstream) # needed for tclap

# check for headers & libs
PKG_CHECK_MODULES(SDL, sdl >= 1.2,[],
	AC_MSG_ERROR([SDL library >= 1.2 not found]))
	
PKG_CHECK_MODULES(LO, liblo >= 0.23,[],
	AC_MSG_ERROR([lo library >= 0.23 not found]))

#########################################
##### Options #####

# debug build switch
# from http://www.bioinf.uni-freiburg.de/~mmann/HowTo/automake.html#debug
AC_MSG_CHECKING([whether to enable the debug build])
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[enable debug build (def=no)])],
	[enable_debug="$enableval"],
	[enable_debug=no])
AC_MSG_RESULT([$enable_debug])

if test x"$enable_debug" = x"yes"; then
	AC_DEFINE([DEBUG],[],[Debug Mode])
	CXXFLAGS="$DEBUG_CXXFLAGS"
else
	AC_DEFINE([NDEBUG],[],[Release Mode])
	CXXFLAGS="$RELEASE_CXXFLAGS"
fi

# allow linking of non-POSIX libs
AM_PROG_AR

# add linker flags for GCC (needed by Linaro/udoo)
if test "x$GCC" = xyes; then
    case "$host_os" in
    	darwin*|mingw*)
        	;;
    	*)
        	LDFLAGS+="-Wl,--no-as-needed"
        	;;
    esac
fi

#########################################
##### Externals #####

# externals with configure scripts
AC_CONFIG_SUBDIRS()

#########################################
##### Output files #####

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	externals/Makefile
	externals/oscframework/Makefile
	externals/xmlframework/Makefile
	externals/tclap/Makefile
	src/Makefile
	src/lib/Makefile
	src/rc-unitd/Makefile
	src/rc-unit-notifier/Makefile
	src/lsjs/Makefile
])
AC_OUTPUT

#########################################
##### Print a summary for the user #####

AC_MSG_RESULT([
	rc-unitd $VERSION is now configured

	Source directory:     $srcdir
	Installation prefix:  $prefix
	C++ compiler:         $CXX $CXXFLAGS
	Debug build:          $enable_debug
])
