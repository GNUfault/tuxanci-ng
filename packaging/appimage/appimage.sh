#!/bin/sh
set -e
VERSION="0.22.0"
echo "Building Tuxánci appimage..."
mkdir -p ../../build tuxanci-$VERSION.AppDir
cmake -S ../../ -B ../../build
make -j$(( $(nproc) + 1 )) -C ../../build
make DESTDIR=../packaging/appimage/tuxanci-$VERSION.AppDir -j$(( $(nproc) + 1 )) -C ../../build install
rm -f tuxanci-$VERSION.AppDir/tuxanci.desktop tuxanci-$VERSION.AppDir/tuxanci.svg
ln -s usr/share/applications/tuxanci.desktop tuxanci-$VERSION.AppDir/tuxanci.desktop
ln -s usr/local/share/pixmaps/tuxanci.svg tuxanci-$VERSION.AppDir/tuxanci.svg
cp templates/AppRun tuxanci-$VERSION.AppDir
rm -f tuxanci-$VERSION-x86_64.AppImage*
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
chmod +x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage ./tuxanci-$VERSION.AppDir
rm -rf appimagetool-x86_64.AppImage* tuxanci-$VERSION.AppDir
