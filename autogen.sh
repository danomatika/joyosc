#! /bin/bash

cd lib/lopack && ./autogen.sh && cd ../../
cd lib/tinyobject && ./autogen.sh && cd ../../

mkdir -p config m4
autoreconf --install --force --verbose
