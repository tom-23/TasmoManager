#!/bin/bash
source scripts/macOS/env.sh

cd build
cqtdeployer -confFile ../shared/tasmomanager.json
cpack