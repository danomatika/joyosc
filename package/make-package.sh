#! /bin/bash

# Prerequisites: Anything that you need to build joyosc on the host platform +
# the PlistBuddy, install_name_tool, codesign, and hdiutil utilities on macOS
# (these should be included in your Command Line Tools or Xcode installation).

# The script can be called from anywhere as long as the proper (relative or
# absolute) path to the script is included. (Usually you'll invoke it from the
# main source directory or the package subdirectory.) It populates the staging
# area (in the $build subdirectory of the current working directory) using
# `make install`, constructs the package contents from it, and zips up the
# package. On macOS and Windows, the required libraries from Homebrew or Mingw
# will be included in the package.

# On Windows, the zip file contains the joyosc application directory, on macOS
# a disk image (dmg file) with the application directory. This can be
# installed in your Applications folder on macOS, or your 'Program Files'
# folder on Windows. The application is launched with the application icon
# (Joyosc.app bundle on macOS, .lnk file on Windows) contained in the joyosc
# folder. Please check the README.md file included in the package for details.

# The package can also be built on Linux, but this is mostly for testing
# purposes right now. In the future, it may become useful as a basis for
# building a flatpak. The Linux package doesn't contain any libraries, so
# you'll have to make sure that libSDL2 and libtinyxml are installed from the
# package manager of your distribution. A .desktop file is included in the
# package which can be used to launch the program. However, you may have to
# adjust the desktop file depending on where you install the package, and copy
# the joyosc.png icon file to some location searched by the desktop
# environment that you use.

# stop on error
set -e

scriptdir=$(dirname $0)
srcdir="$(cd $scriptdir/..; pwd)"
cwd=$(pwd)
build=build
signatureid="-"
keep=false
vername=false
prefix=
verbose=true

# You can pass the joyosc version as the first script argument. By default, it
# is extracted from the configure.ac file.
version=$(grep AC_INIT $srcdir/configure.ac | sed 's/AC_INIT.*\[\([0-9.]*\)\].*/\1/')

# Usually you shouldn't have to edit anything below this line.

#-------------------------------------------------------------------------------

##### parse

# parse options
while [ "$1" != "" ] ; do
    case $1 in
        --sign)
            shift 1
            if [ $# = 0 ] ; then
                echo "--sign option requires a macOS developer SIGNATURE_ID"
                exit 1
            fi
            signatureid="$1"
            ;;
        --keep)
            keep=true
            ;;
        --vername)
            vername=true
            ;;
        --prefix)
            shift 1
            if [ $# = 0 ] ; then
                echo "--prefix option requires an argument"
                exit 1
            fi
            prefix="$1"
            ;;
        -h|--help)
cat <<EOF
Usage: make-package.sh [OPTIONS] [VERSION]

  creates a distributable platform-specific joyosc package

Options:
  -h,--help           display this help message
  --sign SIGNATURE_ID macOS developer id for signing the Joyosc.app bundle,
                      the default is "-" for ad-hoc signing
  --keep              keep build directory, do not delete after creating package
  --vername           use version when naming app dir, ie. joyosc-1.2.3
                      *note* will break Windows .lnk
  --prefix            set environment prefix

Arguments:

  VERSION             optional version string, configure version used if not set

EOF
            exit 0
            ;;
        *)
            break ;;
    esac
    shift 1
done

# parse arguments
if [ "$1" != "" ] ; then
    version="$1"
fi

##### main

os=$(uname -o)
arch=$(uname -m)

# canonical OS/platform name
if [ "$os" == "Darwin" ]; then
    os="macos"
elif [ "$os" == "Msys" ]; then
    os="mingw"
elif [ "$os" == "GNU/Linux" ]; then
    os="linux"
fi

# platform-specific configuration
if [ "$os" == "macos" ]; then
    echo "building macOS package"
    if [ "$prefix" = "" ] ; then
        prefix=/usr/local
    fi
    libdir=$(brew --prefix)
    if [ -f "$libdir/sdl2/lib" ]; then # check lib location
        libs="$libdir/sdl2/lib/libSDL2-2.0.0.dylib $libdir/liblo/lib/liblo.7.dylib $libdir/tinyxml2/lib/libtinyxml2.10.dylib"
    else
        libs="$libdir/lib/libSDL2-2.0.0.dylib $libdir/lib/liblo.7.dylib $libdir/lib/libtinyxml2.10.dylib"
    fi
elif [ "$os" == "mingw" ]; then
    echo "building Windows (mingw) package"
    if [ "$prefix" == "" ] ; then
        if [ -d "/mingw64" ] ; then
            # default mingw
            prefix=/mingw64
        else
            # some other env ie. ucrt64, try grabbing from $PATH
            # if this doesn't work, use --prefix
            prefix=${PATH%%:*}
        fi
    fi
    echo "environment $prefix"
    libdir=$prefix/bin
    libs="$libdir/SDL2.dll $libdir/libgcc_s_seh-1.dll $libdir/liblo-7.dll $libdir/libstdc++-6.dll $libdir/libtinyxml2.dll $libdir/libwinpthread-1.dll"
elif [ "$os" == "linux" ]; then
    echo "building Linux package"
    if [ "$prefix" == "" ] ; then
        prefix=/usr/local
    fi
    # don't package any libraries
    libs=
else
    echo "unrecognized OS $os, exiting" >&2
    exit 1
fi

name=joyosc
if $vername ; then
    app=$name-$version
else
    app=$name
fi
pkgname=$name-$version-$os-$arch
targetdir=$build/$name/$app

# install to staging area
rm -rf $build
make -C $srcdir install DESTDIR=$cwd/$build

# populate the package contents
mkdir -p $targetdir/bin
cp $build/$prefix/bin/* $libs $targetdir/bin
cp -r $build/$prefix/share/doc/joyosc $targetdir/doc
cp $targetdir/doc/joyosc.bmp $targetdir/bin
cp $scriptdir/app/README.md $targetdir

# copy the app icon
if [ "$os" == "macos" ]; then
    # ...and build runner .app bundle

    PLIST_BUDDY=/usr/libexec/PlistBuddy
    appbundle=$targetdir/Joyosc.app
    plist=$targetdir/Joyosc.app/Contents/Info.plist

    mkdir -p $targetdir/Joyosc.app/Contents/MacOS $appbundle/Contents/Resources
    cp $scriptdir/app/mac/Info.plist $appbundle/Contents
    cp $scriptdir/app/mac/Joyosc $appbundle/Contents/MacOS
    cp $targetdir/doc/joyosc.icns $appbundle/Contents/Resources

    # set version identifiers & contextual strings in Info.plist
    $PLIST_BUDDY -c "Set:CFBundleVersion \"$version\"" $plist
    $PLIST_BUDDY -c "Set:CFBundleShortVersionString \"$version\"" $plist
    # remove deprecated key as this will display in Finder instead of version
    $PLIST_BUDDY -c "Delete:CFBundleGetInfoString" $plist

elif [ "$os" == "mingw" ]; then
    cp $scriptdir/app/win/joyosc.lnk $targetdir
elif [ "$os" == "linux" ]; then
    cp $scriptdir/app/linux/joyosc.desktop $targetdir
fi

# zip it up

cd $build

if [ "$os" == "macos" ]; then

    # fix up the lib dependencies
    for lib in $libs; do
        install_name_tool -change $lib @executable_path/$(basename $lib) $name/$app/bin/lsjs
        install_name_tool -change $lib @executable_path/$(basename $lib) $name/$app/bin/joyosc
    done

    # self-sign app
    codesign --deep --sign "$signatureid" $name/$app/Joyosc.app

    # create dmg
    rm -f $app.dmg
    hdiutil create -volname $app -srcfolder $name $app.dmg
    codesign --deep --sign "$signatureid" $app.dmg

    # zip
    zip $pkgname.zip $app.dmg

else # linux/mingw: just zip the package contents
    cd $app
    zip -r $pkgname.zip $app
    mv $pkgname.zip ..
fi

cd $cwd

# move the zip to the package dir
rm -f $scriptdir/$pkgname.zip
mv $build/$pkgname.zip $scriptdir

# remove the staging directory?
if ! $keep ; then
    rm -rf $build
fi
