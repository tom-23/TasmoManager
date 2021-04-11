#!/bin/bash
source scripts/macOS/env.sh
brew update
brew install create-dmg openssl
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
qmake ..
make -j4
sudo make install
cd ../../..