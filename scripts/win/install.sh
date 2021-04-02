#!/usr/bin/bash
set -exo pipefail

# shellcheck disable=SC1091
source scripts/win/env.sh

git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -G "$CMAKE_GENERATOR" -A "$CMAKE_GENERATOR_ARCH" \
    -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
    -DOPENSSL_ROOT_DIR="C:\OpenSSL-v111-Win64"
    
cmake --build .
cmake --install .
cd ../../..