#! /bin/bash

mkdir -p config m4
automake --add-missing
autoreconf -i
