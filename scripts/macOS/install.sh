#!/bin/bash
source scripts/macOS/env.sh
npm install --global create-dmg
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
qmake ..
make -j4
sudo make install
cd ../../..