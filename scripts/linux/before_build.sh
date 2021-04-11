#!/bin/bash
source scripts/linux/env.sh
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ../