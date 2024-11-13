#########################################
##### Prelude #####

AC_INIT([joyosc], [0.7.0], [danomatika@gmail.com], [joyosc], [http://github.com/danomatika/joyosc])
AC_CONFIG_SRCDIR([src/joyosc/main.cpp])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CANONICAL_HOST

# set default if prefix not set (aka returns NONE)
if test "x$prefix" = x"NONE" ; then
	prefix=$ac_default_prefix
fi

#########################################
##### Build settings #####

DEBUG_CXXFLAGS="-O0 -Wall -Werror -Wno-uninitialized"

#########################################
##### Platform #####

case $host in
	*linux*)
		# add linker flags for GCC (needed by Linaro/udoo)
		LDFLAGS="$LDFLAGS -Wl,--no-as-needed"
		;;
	*)
		;;
esac

#########################################
##### Check for programs/libs #####

# check progs
AC_PROG_CXX
AM_PROG_AR # allow non-POSIX libs

# libtool
LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# using c++ compiler and linker
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext], [optional]) # C++11

# check for headers
AC_CHECK_INCLUDES_DEFAULT

# check for headers & libs
PKG_CHECK_MODULES([SDL], [sdl2 >= 2.0], [],
	AC_MSG_ERROR([SDL library >= 2.0 not found]))
	
PKG_CHECK_MODULES([LO], [liblo >= 0.23], [],
	AC_MSG_ERROR([lo library >= 0.23 not found]))

PKG_CHECK_MODULES([TINYXML2], [tinyxml2 >= 6], [],
	AC_MSG_ERROR([tinyxml2 library >= 6.0.0 not found]))

#########################################
##### Build options #####

# debug build switch
# from http://www.bioinf.uni-freiburg.de/~mmann/HowTo/automake.html#debug
AC_MSG_CHECKING([whether to enable the debug build])
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[enable debug build [default=no]])],
	[enable_debug="$enableval"],
	[enable_debug=no])
AC_MSG_RESULT([$enable_debug])

if test x"$enable_debug" = x"yes"; then
	AC_DEFINE([DEBUG], [], [Debug Mode])
	CXXFLAGS="$CXXFLAGS $DEBUG_CXXFLAGS"
else
	AC_DEFINE([NDEBUG], [], [Release Mode])
fi

#########################################
##### Output files #####

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	lib/Makefile
	src/Makefile
	src/joyosc/Makefile
	src/lsjs/Makefile
])
AC_OUTPUT

#########################################
##### Print a summary for the user #####

AC_MSG_RESULT([
=================================================

	$PACKAGE $VERSION is now configured

	Source directory:     $srcdir
	Installation prefix:  $prefix
	C++ compiler:         $CXX $CXXFLAGS $LDFLAGS
	Debug build:          $enable_debug
])
