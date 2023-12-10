#!/usr/bin/env bash

mkdir build
cd build || exit
cmake ..
cmake --build .
cd ..
mv build/host-* .
mv build/client-* .
rm -rf build
