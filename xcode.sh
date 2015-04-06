#!/usr/bin/env bash

if [[ ! -d glitter.xcodeproj ]] ; then
    cmake -G Xcode
fi

/Applications/Xcode.app/Contents/MacOS/Xcode

