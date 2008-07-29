#!/bin/sh

echo "*** GNU/Linux Binary Packager ***"

if [ ! $1 ]; then
	echo "usage: $0 -v version -s"
	echo "eg.: \"$0 -v 0.4.0 -s\" will compile server without -s compile client"
	exit 1
fi
SRV=""
SRV_NAM=""
while getopts v:s arg
do
	case $arg in
		v) VERSION=${OPTARG};;
		s) SRV="-DServer=1"; SRV_NAM="server-";;
		*) echo "no arguments!"; exit 1;;
	esac
done

INSTALL_DIR="pkgs/tuxanci-${SRV_NAM}${VERSION}-linux"
BUILD_DIR="pkgs/build/"
SVN='https://opensvn.csie.org/tuxanci_ng/'
log='linux.log'
error="Error! See pkgs/${log} for more iformation"
mkdir -p pkgs/tuxanci-${SRV_NAM}${VERSION}-linux || ( echo $error; exit 1 )
touch pkgs/${log} || ( echo $error; exit 1 )
echo "" > pkgs/${log} || ( echo $error; exit 1 )

rm -rfv $INSTALL_DIR $BUILD_DIR > pkgs/"${log}" || ( echo $error; exit 1 )

echo "* Fetching files from SVN"
svn export $SVN $BUILD_DIR >> pkgs/"${log}" || ( echo $error; exit 1 )

echo "* Configuring"
echo "#define TUXANCI_VERSION \"${VERSION}\"" > $BUILD_DIR/config.h || ( echo $error; exit 1 )

echo "* Compiling"
prevdir=`pwd`
cd $BUILD_DIR
cmake . -DPREFIX=\\\"..\\\" -DCMAKE_INSTALL_PREFIX:PATH="${prevdir}/${INSTALL_DIR}" ${SRV} -DLIBDIR=\\\"../lib/\\\" >> "${prevdir}"/pkgs/"${log}" || ( echo $error; exit 1 )
make >> "${prevdir}"/pkgs/"${log}" || ( echo $error; exit 1 )
sudo make install >> "${prevdir}"/pkgs/"${log}" || ( echo $error; exit 1 )
cd $prevdir

echo "* Packaging"
tar cjf "${INSTALL_DIR}".tar.bz2 $INSTALL_DIR || ( echo $error; exit 1 )

echo "** GNU/Linux binary package is ready!"

