#!/bin/bash
source scripts/macOS/env.sh

if test -d build/TasmoManager.app; then
    export PATH=${CMAKE_PREFIX_PATH}/bin:${PATH}
    cd build
    macdeployqt TasmoManager.app -always-overwrite -verbose=1
    python ../lib/macdeployqtfix/macdeployqtfix.py --quiet "TasmoManager.app/Contents/MacOS/TasmoManager ${CMAKE_PREFIX_PATH}"
    create-dmg --help
    create-dmg TasmoManager.app 
    for f in *.dmg; do mv -- "$f" "macOS-${APP}-diskimage.dmg"; done
    pkgbuild --root "TasmoManager.app" \
         --install-location "/Applications/TasmoManager.app" \
         --identifier "com.tombutcher.tasmomanager.pkg" \
         --version "${APP_VERSION}" \
         "macOS${APP}-installer.pkg"
else
    true
fi

