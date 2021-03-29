export QTDIR=$HOME/Qt/5.12.10/clang_64

export PATH=$QTDIR/bin:$PATH

export OPENSSL_ROOT_DIR=/usr/local/opt/openssl@1.1

export BUILD_PATH=$APPVEYOR_BUILD_FOLDER/build
export CMAKE_PREFIX_PATH=$QTDIR

APP_VERSION=$(git describe --tags --always HEAD)
export APP_VERSION
export APP=tasmomanager-$APP_VERSION