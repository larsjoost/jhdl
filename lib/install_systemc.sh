#!/bin/bash

cd systemc

mkdir -p build

cd build

cmake .. -DCMAKE_CXX_STANDARD=14 -DCMAKE_BUILD_TYPE=Debug

make

sudo make install
