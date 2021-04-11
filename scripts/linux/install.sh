#!/bin/bash
source scripts/linux/env.sh
sudo apt install mesa-common-dev libglu1-mesa-dev
sudo snap install cqtdeployer
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
qmake ..
make -j4
sudo make install
cd ../../..