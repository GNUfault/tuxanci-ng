#!/bin/sh
set -e -x

NAME="tuxanci"
VERSION="0.22.2"
ARCH="amd64"
PKG_DIR="${NAME}_${VERSION}_${ARCH}"

mkdir -p "$PKG_DIR/DEBIAN"

cmake -S ../../ -B ../../build -DCMAKE_INSTALL_PREFIX=/usr
make -j$(( $(nproc) + 1 )) -C ../../build

cat <<EOF > "$PKG_DIR/DEBIAN/control"
Package: $NAME
Version: $VERSION
Section: games
Priority: optional
Architecture: $ARCH
Maintainer: GNUfault
Depends: libc6, libsdl1.2debian, libsdl-image1.2, libsdl-mixer1.2, libsdl-ttf2.0-0, libzip5, libopengl0
Description: Tuxanci is first tux shooter inspired by a well-known Czech game Bulanci.
EOF

make DESTDIR=../packaging/deb/$PKG_DIR -j$(( $(nproc) + 1 )) -C ../../build install

dpkg-deb --build --root-owner-group "$PKG_DIR"

rm -rf $PKG_DIR ../../build
