#!/bin/bash

mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
mv build/mydaemon mydaemon
rm -rf build
