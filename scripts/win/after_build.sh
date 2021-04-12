#!/usr/bin/bash
set -exo pipefail

# shellcheck disable=SC1091
source scripts/win/env.sh

Source=$APPVEYOR_BUILD_FOLDER
Destination=$APPVEYOR_BUILD_FOLDER/$APP
Executable=$Destination/tasmomanager.exe

mkdir -p "$Destination"

cmake --install "$BUILD_PATH" --config Release --prefix "$Destination" --verbose

cp -v "C:\OpenSSL-v111-Win64\bin\libcrypto-1_1-x64.dll" "$Destination"
cp -v "C:\OpenSSL-v111-Win64\bin\libssl-1_1-x64.dll" "$Destination"

cp -v "$Source/LICENSE" "$Destination"
cp -v "$Source/README.md" "$Destination"

"$QTDIR/bin/windeployqt" --help
"$QTDIR/bin/windeployqt" \
    --no-system-d3d-compiler \
    --no-quick \
    "$Executable"

# Create and upload portable zip file.
7z a "windows-$APP-portable.zip" -r "$Destination"
appveyor PushArtifact "windows-$APP-portable.zip" -DeploymentName "TasmoManager Portable"

choco install -y InnoSetup
cmd " /c C:/ProgramData/chocolatey/bin/ISCC.exe /O$APPVEYOR_BUILD_FOLDER /DAppVersion=$APP_VERSION /DRoot=$Destination /DSource=$Source $Source/Shared/tasmomanager.iss"
