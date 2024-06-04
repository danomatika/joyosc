#! /bin/sh
#
# clean autotools files, use this in addition to make distclean
#

rm -rf autom4te.cache config dist
rm -f m4/libtool.m4 m4/lt*.m4
rm -f INSTALL aclocal.m4 configure config.log config.status libtool
