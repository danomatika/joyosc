INSTALL_DIR=/usr/lib/pd
BUILD_DIR=./
INSTALL_CMD="cp -xR"

mkdir -p $INSTALL_DIR
mkdir -p $INSTALL_DIR/extra
mkdir -p $INSTALL_DIR/extra/unit-patches
$INSTALL_CMD $BUILD_DIR $INSTALL_DIR/extra/unit-patches
