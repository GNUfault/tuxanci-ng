#!/bin/sh

PROJECT="tuxanci-ng"
TMP_DIR="tmp"
DEB_ARCH="i386"
DEB_NAME="tuxanci-ng_svn"

SRC_DIR="../../.."

rm -rf $TMP_DIR


mkdir $TMP_DIR
mkdir $TMP_DIR/DEBIAN
mkdir $TMP_DIR/usr
mkdir $TMP_DIR/usr/bin
mkdir $TMP_DIR/usr/share
mkdir $TMP_DIR/usr/share/applications

cp control $TMP_DIR/DEBIAN/
cp $PROJECT.desktop $TMP_DIR/usr/share/applications

cd $TMP_DIR

echo -e "2.0\n" > DEBIAN/debian-binary

mv $SRC_DIR/config.h $SRC_DIR/zal_config.h
mv $SRC_DIR/deb_config.h $SRC_DIR/config.h

make -C $SRC_DIR clean 
make -C $SRC_DIR 
make -C $SRC_DIR DESTDIR="./packaging/deb/$TMP_DIR/usr" install;

mv $SRC_DIR/config.h $SRC_DIR/deb_config.h
mv $SRC_DIR/zal_config.h $SRC_DIR/config.h

md5sum `find ./ -type f | awk '/.\// { print substr($0, 3) }'` > DEBIAN/md5sums

cd ..
rm -rf *.deb
dpkg -b ./$TMP_DIR $DEB_NAME`cat r`_$DEB_ARCH.deb
