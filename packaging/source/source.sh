#!/bin/sh

echo "*** Source Packager ***"

if [ ! $1 ]; then
	echo "usage: $0 version"
	exit 1
fi

VERSION=$1
INSTALL_DIR="pkgs/tuxanci-${VERSION}-src"
SVN='https://opensvn.csie.org/tuxanci_ng/'
error="Error! See pkgs/src.log for more iformation"

mkdir pkgs -p
touch pkgs/src.log
2&>1

rm -rfv $INSTALL_DIR > pkgs/src.log || `echo $error; exit 1`


echo "* Fetching files from SVN"
svn export $SVN pkgs/tuxanci-0.20.0-src >> pkgs/src.log || `echo $error; exit 1`

echo "* Configuring"
echo "#define TUXANCI_VERSION \"${VERSION}\"" > $INSTALL_DIR/config.h || `echo $error; exit 1`

rm -fv $INSTALL_DIR/ci >> pkgs/src.log || `echo $error; exit 1`

echo "* Packaging"
tar cjf $INSTALL_DIR.tar.bz2 $INSTALL_DIR || `echo $error; exit 1`

echo "** Source package is ready!"

