#!/bin/bash
source scripts/macOS/env.sh

cd build
cqtdeployer -bin tasmomanager -extraLibs qmqtt -targetDir ./Distro
cd Distro
tar -czvf "../linux-${APP}-dist.tar.gz" *
cd ..
appveyor PushArtifact "linux-${APP}-dist.tar.gz" -DeploymentName "TasmoManager Linux Distribution"

cpack .
cd packages
for f in *.deb; do sudo dpkg -i "$f"; done