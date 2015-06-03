#!/usr/bin/env bash

if [[ ! -f CMakeLists.txt || ! -d src ]] ; then
    echo "Are you in the correct directory? Only execute this script as ./clean.sh!"
    exit 1
fi

rm -rf CMakeCache.txt \
       CTestTestfile.cmake \
       CMakeFiles \
       Makefile \
       libbson-lib-prefix \
       cmake_install.cmake \
       doc/html \
       doc/Doxyfile \
       doxygen-prefix \
       libdocument-ex1 \
       libdocument.dylib \
       libdocument.build \
       libdocument.xcodeproj \
       libbson-lib-prefix \
       python-lib-prefix \
       googletest-lib-prefix \
       CMakeScripts \
       src/doxygen \
       src/googletest \
       src/libbson \
       Debug

