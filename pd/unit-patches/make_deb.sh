#! /bin/bash

# make a Debian package

NAME="rc-unitd-patches"

# library/build depenencies
PD="puredata"

# exit if checkinstall is not installed
if [ ! -e "/usr/bin/checkinstall" ] ; then
	echo "cannot build deb package, checkinstall is not installed"
	sleep 2
	exit 0
fi

# build the package
checkinstall -D -y --install=no --pkgname="$NAME" --pkgversion="0.1.0" \
--requires="$PD" --maintainer="danomatika@gmail.com" --deldoc=yes \
--strip=no --stripso=no --deldesc=yes --delspec=yes ./install_for_deb.sh

