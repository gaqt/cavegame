#!/bin/bash

mkdir -p $1
cd lib/raylib/src
make PLATFORM=PLATFORM_DESKTOP
cd ../../..
mv lib/raylib/src/libraylib.a $1/libraylib.a
