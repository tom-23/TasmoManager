#!/bin/bash
source scripts/macOS/env.sh

cd build
cqtdeployer -bin tasmomanager -extraLibs qmqtt -targetDir ./Distro
cpack .
cd packages
for f in *.deb; do sudo dpkg -i "$f"; done