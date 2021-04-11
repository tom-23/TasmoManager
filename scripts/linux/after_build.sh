#!/bin/bash
source scripts/macOS/env.sh

cd build
cqtdeployer
cqtdeployer -bin tasmomanager qif

7z a "linux-$APP-installer.zip" -r "DistributionKit"
