#!/usr/bin/env bash

if [[ ! -d libdocument.xcodeproj ]] ; then
    cmake -G Xcode
fi

/Applications/Xcode.app/Contents/MacOS/Xcode

