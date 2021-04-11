#!/bin/bash
source scripts/linux/env.sh
sudo snap install cqtdeployer
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
qmake ..
make -j4
sudo make install
cd ../../..