##### Prelude #####
AC_INIT([rc-unitd], [0.1.0], [danomatika@gmail.com])
AC_CONFIG_SRCDIR([src/rc-unitd/main.cpp])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

##### Check for programs/libs #####
# check progs
AC_PROG_CXX
AC_PROG_RANLIB

# using c++ compiler and linker
AC_LANG_CPLUSPLUS

# check for headers
AC_HEADER_STDC
AC_CHECK_HEADER([linux/input.h],[],
	AC_MSG_ERROR([linux/input.h not found]))
AC_CHECK_HEADER([lo/lo.h],[],
	AC_MSG_ERROR([lo/lo.h not found]))

# check for headers & libs
PKG_CHECK_MODULES(GLIB2, glib-2.0 >= 2.14,[],
	AC_MSG_ERROR([glib-2.0 library not found]))
PKG_CHECK_MODULES(DBUS, dbus-1 >= 0.60,[],
	AC_MSG_ERROR([dbus-1 library not found]))
#PKG_CHECK_MODULES(LIBLO, liblo0 >= 0.22,[],
#	AC_MSG_ERROR([liblo0 library not found]))
AC_CHECK_LIB([lo], [lo_arg_pp],
	AC_SUBST([LIBLO_LIBS], ["-llo"]),
	AC_MSG_ERROR([liblo library not found]))

# check for optional libs,
# from http://www.gnu.org/software/autoconf/manual/html_node/External-Software.html

# check for configure option and set makefile conditional
AC_ARG_ENABLE([cwiid],
	[AS_HELP_STRING([--enable-cwiid],
		[enable experimental wiimote support using libcwiid])],
	[enable_cwiid=yes],
	[enable_cwiid=no])
AM_CONDITIONAL([ENABLE_CWIID], [test "x$enable_cwiid" = xyes])

# check for lib and set link flags and defines, fail if lib not installed
CWIID_LIBS=
AS_IF([test "x$enable_cwiid" = xyes],
	[AC_CHECK_LIB([cwiid], [cwiid_open],
		[AC_SUBST([CWIID_LIBS], ["-lcwiid"])
			AC_DEFINE([HAVE_CWIID], [1],
				[Define if you have libcwiid])
			],
			[AC_MSG_FAILURE([--with-cwiid was given but libcwiid not found])],
			[])])

# arg to disable installation of config files, need to disable this for distcheck
AC_ARG_ENABLE([config],
  [AS_HELP_STRING([--disable-config],
    [disable installation of config files])])
AM_CONDITIONAL([ENABLE_CONFIG], [test "x$enable_config" != xno]) 

##### Output files #####
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/lib/Makefile
	src/lsjs/Makefile
	src/rc-unitd/Makefile
	src/rc-unit-notifier/Makefile
])
AC_OUTPUT

# Print a summary for the user
AC_MSG_RESULT([
	rc-unitd $VERSION is now configured

	Source directory:     $srcdir
	Installation prefix:  $prefix
	C++ compiler:         $CXX $CXXFLAGS

	cwiid wiimote support: $enable_cwiid
])
