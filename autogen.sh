#! /bin/sh
#
# generate autotools files
#

aclocal --force -I m4
automake --add-missing
autoreconf --install --force
