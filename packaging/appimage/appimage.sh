#!/bin/sh

# Exit on error
set -e

# Tuxánci version
VERSION="0.22.0"

# Make build directory
mkdir -p ../../build tuxanci-$VERSION.AppDir

# Configure
cmake -S ../../ -B ../../build

# Compile
make -j$(( $(nproc) + 1 )) -C ../../build

# Install
make DESTDIR=../packaging/appimage/tuxanci-$VERSION.AppDir -j$(( $(nproc) + 1 )) -C ../../build install

# Remove build artifacts
rm -f tuxanci-$VERSION.AppDir/tuxanci.desktop tuxanci-$VERSION.AppDir/tuxanci.svg tuxanci-$VERSION-x86_64.AppImage*

# Symlink desktop file
ln -s usr/share/applications/tuxanci.desktop tuxanci-$VERSION.AppDir/tuxanci-$VERSION.desktop

# Symlink icon
ln -s usr/local/share/pixmaps/tuxanci.svg tuxanci-$VERSION.AppDir/tuxanci.svg

# Copy AppDir
cp templates/AppRun tuxanci-$VERSION.AppDir

# Download appimagetool
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage

# Make appimagetool executable
chmod +x appimagetool-x86_64.AppImage

# Run appimagetool
./appimagetool-x86_64.AppImage ./tuxanci-$VERSION.AppDir

# Remove build artifacts
rm -rf appimagetool-x86_64.AppImage* tuxanci-$VERSION.AppDir
