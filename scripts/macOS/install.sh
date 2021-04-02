#!/usr/bin/env bash
source scripts/macOS/env.sh
brew update
brew install create-dmg openssl
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
cmake 
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
cmake --build .
cmake --install .
cd ../../..