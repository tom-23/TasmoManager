#!/bin/bash
source scripts/macOS/env.sh

cd build
cqtdeployer -bin tasmomanager -targetDir ./Distro
make package