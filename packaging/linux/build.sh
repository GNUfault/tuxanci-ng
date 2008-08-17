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
	echo "-a 32b \"compile 32b ELF (64b for 64b ELF)\""
	echo "-r 183 \"create packages from svn revision 183\""
	echo "!if no version is specified svn version is created!"
	echo "!if no arch is specified 64b is used!"
	echo "!if no revision is specified HEAD is used!"
	exit 1
fi
###############################################################################
# ARGUMENT PASSING
###############################################################################
VERSION="9999"
ARCH="64b"
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
SVN="https://opensvn.csie.org/tuxanci_ng/"
BUNDLE_PREFIX="${HOME}/tuxanci-bundle"
LOG="${BUNDLE_PREFIX}/linux.log"
SOURCE="${BUNDLE_PREFIX}"/"${APPNAME}"_source_"${VERSION}"
D="${BUNDLE_PREFIX}/${APPNAME}"
OPTIONS="client server"
ERROR_MESSAGE="Check ${LOG}, cause i was unable to finish my stuff correctly!"
CMAKE_PARAMS="-DBundle=1 -DTUXANCI_VERSION=${VERSION} -DCMAKE_BUILD_TYPE=Release"
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
cmake . ${CMAKE_PARAMS}  >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
make >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
make install DESTDIR="${D}_${Y}_${VERSION}-${ARCH}/" >> "${LOG}" || ( echo "${ERROR_MESSAGE}"; exit 1 )
cd "${D}"_"${Y}"_"${VERSION}"-"${ARCH}"/
`which strip` --strip-unneeded lib/tuxanci/* bin/* >> "${LOG}"
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
find ./ -maxdepth 1 -type f -name \*.tar.bz2 -print | while read FILE ; do
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
