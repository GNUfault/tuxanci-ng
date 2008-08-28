#!/usr/bin/env sh
# Created by Tomas Chvatal (Scarabeus_IV)
###############################################################################
echo "<******************************>"
echo "<GNU/Linux TUXANCI Packager>"
echo "<******************************>"
# This tool is only for tuxanci game developers, don't use it for yourself
###############################################################################
# HELP
###############################################################################
if [[ $1 == "--help" ]]; then
	echo "usage: $0 -v version -a arch -r revision"
	echo "-v 0.2.5 \"compile version 0.2.5\""
	echo "-a i386 \"compile 32b ELF (64b for 64b ELF)\""
	echo "-r 183 \"create packages from svn revision 183\""
	echo "!if no version is specified svn version is created!"
	echo "!if no arch is specified 64b is used!"
	echo "!if no revision is specified HEAD is used!"
	exit 1
fi
###############################################################################
# ARGUMENT PASSING
###############################################################################
VERSION="svn"
ARCH="amd64"
SVN_REV=""
while getopts v:a:r: arg ; do
	case $arg in
		v) VERSION=${OPTARG};;
		a) ARCH=${OPTARG};;
		r) SVN_REV="-r ${OPTARG}";;
	esac
done
###############################################################################
# VARIABLES
###############################################################################
APPNAME="tuxanci"
SVN="http://opensvn.csie.org/tuxanci_ng/"
BUNDLE_PREFIX="${HOME}"/tuxanci-bundle
LOG="${BUNDLE_PREFIX}"/linux.log
SOURCE="${BUNDLE_PREFIX}"/"${APPNAME}"_source_"${VERSION}"
D="${BUNDLE_PREFIX}"/"${APPNAME}"
OPTIONS="client server"
ERROR_MESSAGE="Check ${LOG}, cause i was unable to finish my stuff correctly!"
CMAKE_PARAMS="-DNLS=1 -DTUXANCI_VERSION=${VERSION} -DCMAKE_BUILD_TYPE=Release"
###############################################################################
# PREPARING ENVIROMENT
###############################################################################
mkdir -p "${BUNDLE_PREFIX}" || ( echo "I was unable to create working directory"; exit 1 )
rm -rf "${BUNDLE_PREFIX}"/*
cd "${BUNDLE_PREFIX}"
touch "${LOG}" || ( echo "I was unable to create log file"; exit 1 )
echo "" > "${LOG}"	# LOG CLEANUP
echo "<Downloading files from SVN repository>"
echo "<******************************>"
svn export $SVN ${SVN_REV} ${SOURCE} >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
rm -rf ${SOURCE}/packaging
###############################################################################
# CREATE SOURCE PACKAGE
###############################################################################
tar cjf "${APPNAME}"_source_"${VERSION}".tar.bz2 "${APPNAME}"_source_"${VERSION}"/
###############################################################################
# DEBIAN PACKAGE ENVIROMENT CREATION
###############################################################################
for Y in ${OPTIONS}; do
DEST_DEB="${BUNDLE_PREFIX}"/"${APPNAME}-${Y}"-deb
echo "<******************************>"
echo "<Building ${Y} ${ARCH} DEBPKG>"
echo "<******************************>"
mkdir "${DEST_DEB}"
mkdir -p "${DEST_DEB}"/DEBIAN
mkdir -p "${DEST_DEB}"/usr/share/applications/
# create desktop file and other debian related
echo "2.0" > "${DEST_DEB}"/DEBIAN/debian-binary
echo "[Desktop Entry]
Name=${APPNAME}-${Y}-${VERSION}
Comment=game tuxanci
Exec=/usr/games/bin/${APPNAME}-${VERSION}
Terminal=false
Type=Application
Icon=/usr/games/share/tuxanci-svn/images/tuxanci.png
Encoding=UTF-8
Categories=Game" > "${DEST_DEB}"/usr/share/applications/tuxanci.desktop
echo "Package: ${APPNAME}-${Y}-${VERSION}
Version: 0.2.1
Section: Game
Priority: optional
Depends: libc6 (>= 2.2.4-4), libsdl1.2debian (>= 1.2), libsdl-image1.2 (>= 1.2), libsdl-mixer1.2 (>= 1.2), libsdl-ttf2.0-0 (>= 2.0), gettext
Architecture: ${ARCH}
Installed-Size: 4993
Maintainer: Tomas Chvatal <tomas.chvatal@gmail.com>
Description: Game tuxanci next generation version 0.2.1
 .
 http://www.tuxanci.org" > "${DEST_DEB}"/DEBIAN/control
###############################################################################
# BUILDING DEBIAN PACKAGES
###############################################################################
CMAKE_PARAMS="${CMAKE_PARAMS} -DCMAKE_INSTALL_PREFIX=/usr/games/"
if [[ ${Y} == "server" ]]; then
	CMAKE_PARAMS="${CMAKE_PARAMS} -DServer=1"
fi
cd "${SOURCE}"
rm CMakeCache.txt || ( echo "I was unable to cleanup CMakeCache.txt"; exit 1; )
make clean >> "${LOG}"
cmake . ${CMAKE_PARAMS}  >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
make >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
make install DESTDIR="${DEST_DEB}" >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
cd "${DEST_DEB}"/
`which strip` --strip-unneeded usr/games/lib/${APPNAME}-${VERSION}/* usr/games/bin/* >> "${LOG}"
md5sum `find . -type f | awk '/.\// { print substr($0, 3) }'` > DEBIAN/md5sums
cd "${BUNDLE_PREFIX}"
Y_N=$(echo ${Y} |tr s S |tr c C)
dpkg-deb -b "${DEST_DEB}" ${APPNAME}${Y_N}_${VERSION}-1_${ARCH}.deb
rm -rf "${DEST_DEB}"
done
CMAKE_PARAMS="-DBundle=1 -DNLS=1 -DTUXANCI_VERSION=${VERSION} -DCMAKE_BUILD_TYPE=Release" # duplicating due to override
###############################################################################
# BUILDING BINARIES
###############################################################################
for Y in ${OPTIONS}; do
echo "<******************************>"
echo "<Building ${Y} ${ARCH}>"
echo "<******************************>"
if [[ ${Y} == "server" ]]; then
	CMAKE_PARAMS="${CMAKE_PARAMS} -DServer=1"
fi
cd "${SOURCE}"
rm CMakeCache.txt || ( echo "I was unable to cleanup CMakeCache.txt"; exit 1; )
make clean >> "${LOG}"
cmake . ${CMAKE_PARAMS} >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
make >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
make install DESTDIR="${D}_${Y}_${VERSION}-${ARCH}/" >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
cd "${D}"_"${Y}"_"${VERSION}"-"${ARCH}"/
`which strip` --strip-unneeded lib/${APPNAME}-${VERSION}/* bin/* >> "${LOG}"
done
###############################################################################
# DESTRUCTION OF SRC
###############################################################################
cd "${BUNDLE_PREFIX}"
rm -rf "${SOURCE}"
###############################################################################
# BUILDING TARS
###############################################################################
echo "<Creating tar.bz2 archives>"
echo "<******************************>"
for Y in ${OPTIONS} ; do
	tar cjf "${APPNAME}"_"${Y}"_"${VERSION}"-"${ARCH}".tar.bz2 "${APPNAME}"_"${Y}"_${VERSION}-"${ARCH}"/*
done
###############################################################################
# DESTRUCTION OF UNPACKED BINARIES
###############################################################################
for Y in ${OPTIONS} ; do
	rm -rf "${APPNAME}"_"${Y}"_${VERSION}-"${ARCH}"/
done
###############################################################################
# SHOW WHAT HAVE WE DONE
###############################################################################
echo "<What have i created :>"
echo "<******************************>"
find ./ -maxdepth 1 -type f \( -name \*.tar.bz2 -print , -name \*.deb -print \) | while read FILE ; do
	echo "FILE: ${FILE}"
	echo "      SIZE: $(`which du` -h ${FILE} |`which awk` -F' ' '{print $1}')"
	echo "    MD5SUM: $(`which md5sum` ${FILE} |`which awk` -F' ' '{print $1}')"
	echo "   SHA1SUM: $(`which sha1sum` ${FILE} |`which awk` -F' ' '{print $1}')"
	echo
done
echo
echo
echo "<GNU/Linux binary packages are ready>"
echo
echo
