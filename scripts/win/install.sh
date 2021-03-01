#!/usr/bin/bash
set -exo pipefail

# shellcheck disable=SC1091
source scripts/win/env.sh

ls "$QTDIR/bin"
ls "$QTDIR/lib/cmake"

"$QTDIR/bin/qmake" --version

git submodule update --init --recursive

cd lib/qmqtt
mkdir build

cd build
cmd "scripts/win/install_qmqtt.bat"
cd ..\..\..