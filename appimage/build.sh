#!/bin/sh
set -e

CPUS=$(nproc)
VERSION=0.23.0

rm -rf io.github.GNUfault.tuxanci-$VERSION-x86_64.AppImage AppDir .AppImage

cd ..

rm -rf build
mkdir build
cd build

cmake .. -DCMAKE_INSTALL_PREFIX=../appimage/AppDir/usr

make -j$CPUS

make install

cd ../appimage

cp AppRun AppDir

chmod +x AppDir/AppRun

cp AppDir/usr/share/applications/io.github.GNUfault.tuxanci.desktop AppDir
cp AppDir/usr/share/pixmaps/io.github.GNUfault.tuxanci.svg AppDir

appimagetool AppDir

mv Tuxánci-x86_64.AppImage io.github.GNUfault.tuxanci-$VERSION-x86_64.AppImage