#!/bin/bash
cd src && rm -rf build && mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
