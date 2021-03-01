#!/usr/bin/bash
set -exu
export PATH=$QTDIR/bin:$PATH

export BUILD_PATH=$APPVEYOR_BUILD_FOLDER/build
export DEPENDENCY_BUILD_PATH=$DEPENDENCY_PATH/build
export DEPENDENCY_PATH=$APPVEYOR_BUILD_FOLDER/dependencies
export DOWNLOADS_PATH=$APPVEYOR_BUILD_FOLDER/downloads

export CMAKE_PREFIX=$INSTALL_PREFIX/lib/cmake
export CMAKE_PREFIX_PATH="$CMAKE_PREFIX:$INSTALL_PREFIX/share/ECM/cmake":$QTDIR/lib/cmake

APP_VERSION=$(git describe --tags --always HEAD)
export APP_VERSION
export APP=tasmomanager-$APP_VERSION

mkdir -p "$INSTALL_PREFIX"
mkdir -p "$DEPENDENCY_PATH"
mkdir -p "$DOWNLOADS_PATH"