#!/usr/bin/bash
set -exo pipefail

# shellcheck disable=SC1091
source utils/appveyor/env.sh

export PATH=$PATH:$INSTALL_PREFIX/bin

cmake -B"$BUILD_PATH" -DCMAKE_BUILD_TYPE=Release \
    -G "$CMAKE_GENERATOR" -A "$CMAKE_GENERATOR_ARCH" \
    -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
    -DCMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION=. \