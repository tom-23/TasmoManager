#!/bin/bash
source scripts/linux/env.sh
sudo apt-get -y install mesa-common-dev libglu1-mesa-dev rpm
sudo snap install cqtdeployer
sudo snap connect cqtdeployer:system-backup :system-backup
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
qmake ..
make -j4
sudo make install
cd ../../..