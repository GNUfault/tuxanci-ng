# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit games cmake-utils subversion

DESCRIPTION="Tuxanci is first cushion shooter based on well-known Czech game Bulanci."
HOMEPAGE="http://www.tuxanci.org/"
#SRC_URI=""
ESVN_REPO_URI="http://opensvn.csie.org/tuxanci_ng"
LICENSE="GPL2"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE="alsa dedicated"
# alsa is used only when building client

DEPEND="!dedicated? (
			>=media-libs/libsdl-1.2.10
			>=media-libs/sdl-ttf-2.0.7
			>=media-libs/sdl-image-1.2.6-r1
			alsa? (
				>=media-libs/sdl-mixer-1.2.7
			)
		)
		"
RDEPEND="${DEPEND}"
S=${WORKDIR}/${PN}
src_unpack() {
	subversion_src_unpack
	cd "${S}"
	# setting proper prefix
	sed -i \
		-e "s:PATH_DIR\tPREFIX:PATH_DIR:" src/base/path.h \
		|| die "sed config.h failed!"
	sed -i \
		-e "s:share/tuxanci:${GAMES_DATADIR}/${PN}:" src/base/path.h \
		|| die "sed config.h failed!"
	sed -i \
		-e "s:CMAKE_INSTALL_DATADIR share/:CMAKE_INSTALL_DATADIR /usr/share/games/:" CMakeLists.txt \
		|| die "sed CMakeLists.txt failed!"
}
src_compile() {
	local mycmakeargs
	use alsa || mycmakeargs="${mycmakeargs} -DNO_Audio=1"
	use dedicated && mycmakeargs="${mycmakeargs} -DNO_Server=1"
	mycmakeargs="${mycmakeargs} -DPREFIX=\\\"/usr/games\\\"	-DCMAKE_INSTALL_PREFIX:PATH=/usr/games"
	cmake-utils_src_compile
}
src_install() {
	cmake-utils_src_install
}
