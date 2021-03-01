#!/usr/bin/env bash

brew update
brew install create-dmg
export CMAKE_PREFIX_PATH=$HOME/Qt/5.12.10/clang_64
export PATH=${CMAKE_PREFIX_PATH}/bin:${PATH} ;
git submodule update --init --recursive
cd lib/qmqtt
mkdir build
cd build
qmake ..
make -j4
make install
cd ../../..