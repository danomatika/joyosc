#! /bin/bash
#
# clean autotools files, use this in addition to make distclean
#

cd lib/lopack && ./autoclean.sh && cd ../../
cd lib/tinyobject && ./autoclean.sh && cd ../../

rm -rf autom4te.cache config dist m4
rm -f INSTALL aclocal.m4 configure config.log config.status libtool
