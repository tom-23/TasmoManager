export QTDIR=$HOME/Qt/5.15.2/gcc_64

export PATH=$QTDIR/bin:$PATH

export BUILD_PATH=$APPVEYOR_BUILD_FOLDER/build
export CMAKE_PREFIX_PATH=$QTDIR

APP_VERSION=$(git describe --tags --always HEAD)
export APP_VERSION
export APP=tasmomanager-$APP_VERSION