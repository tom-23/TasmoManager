#!/usr/bin/bash
set -exu

export QTDIR="C:\Qt\5.15\msvc2019_64"

export CMAKE_GENERATOR="Visual Studio 16 2019"
export CMAKE_GENERATOR_ARCH="x64"

export PATH=$QTDIR/bin:$PATH

export OPENSSL_ROOT_DIR="C:\OpenSSL-v111-Win64"

export BUILD_PATH=$APPVEYOR_BUILD_FOLDER/build
export CMAKE_PREFIX_PATH=$QTDIR/lib/cmake

APP_VERSION=$(git describe --tags --always HEAD)
export APP_VERSION
export APP=tasmomanager-$APP_VERSION