#!/bin/sh

PROJECT="tuxanci-ng"
DEB_DIR="deb"
DEB_ARCH="i386"
DEB_NAME="tuxanci-ng_svn"

SRC_DIR="../myusr"

rm -rf $DEB_DIR

mkdir $DEB_DIR
mkdir $DEB_DIR/DEBIAN
mkdir $DEB_DIR/usr
mkdir $DEB_DIR/usr/bin
mkdir $DEB_DIR/usr/share
mkdir $DEB_DIR/usr/share/applications

cp control deb/DEBIAN/
cp $PROJECT.desktop deb/usr/share/applications

cd deb

echo -e "2.0\n" > DEBIAN/debian-binary

cp $SRC_DIR/bin/$PROJECT usr/bin/
cp -rf $SRC_DIR/share/$PROJECT usr/share/

md5sum `find ./ -type f | awk '/.\// { print substr($0, 3) }'` > DEBIAN/md5sums

cd ..
rm -rf *.deb
dpkg -b ./deb $DEB_NAME`cat r`_$DEB_ARCH.deb
