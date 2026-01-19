#!/bin/sh
set -e
echo "Building Tuxánci appimage..."
mkdir -p ../../build tuxanci.AppDir
cmake -S ../../ -B ../../build
make -j$(( $(nproc) + 1 )) -C ../../build
make DESTDIR=../packaging/appimage/tuxanci.AppDir -j$(( $(nproc) + 1 )) -C ../../build install
rm -f tuxanci.AppDir/tuxanci.desktop tuxanci.AppDir/tuxanci.svg
ln -s usr/share/applications/tuxanci.desktop tuxanci.AppDir/tuxanci.desktop
ln -s usr/local/share/pixmaps/tuxanci.svg tuxanci.AppDir/tuxanci.svg
cp templates/AppRun tuxanci.AppDir
rm -f tuxanci-x86_64.AppImage*
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage ./tuxanci.AppDir
rm -rf appimagetool-x86_64.AppImage* tuxanci.AppDir
