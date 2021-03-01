if test -d src/TasmoManager.app; then
    export PATH=${CMAKE_PREFIX_PATH}/bin:${PATH}
    cd src
    macdeployqt TasmoManager.app -always-overwrite -verbose=1
    python ../../lib/macdeployqtfix/macdeployqtfix.py --quiet TasmoManager.app/Contents/MacOS/TasmoManager ${CMAKE_PREFIX_PATH}
    ../../scripts/macOS/bundle_macOS.sh
    cd ..
else
    true
fi

create-dmg --volname TasmoManager \
    --volicon "../../resources/appicon.icns" \
    --background "../../shared/dmgbackground.png" \
    --window-pos 200 120 \
    --window-size 500 300 \
    --icon-size 160 \
    --icon "TasmoManager.app" 120 88 \
    --hide-extension "TasmoManager.app" \
    --app-drop-link 368 88 \
    "TasmoManager.dmg" \
    "TasmoManager.app"