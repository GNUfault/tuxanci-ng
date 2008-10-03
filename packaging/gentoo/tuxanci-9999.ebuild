# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit games cmake-utils git

DESCRIPTION="Tuxanci is first cushion shooter inspired by game Bulanci."
HOMEPAGE="http://www.tuxanci.org/"
EGIT_REPO_URI="git://repo.or.cz/tuxanci.git"
#SRC_URI="http://download.${PN}.org/${PV}/${P}-src.tar.bz2"
LICENSE="GPL-2"

SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="alsa debug dedicated nls"
# alsa is used only when building client

DEPEND="!dedicated? (
			>=media-libs/libsdl-1.2.10[X]
			>=media-libs/sdl-ttf-2.0.7[X]
			>=media-libs/sdl-image-1.2.6-r1[png]
			alsa? (
				>=media-libs/sdl-mixer-1.2.7[vorbis]
			)
		)
	dev-libs/zziplib[sdl]
	>=dev-util/cmake-2.6.0
	nls? ( sys-devel/gettext )"

S="${WORKDIR}"/"${PN}"

src_configure() {
	local mycmakeargs
	use alsa || mycmakeargs="${mycmakeargs} -DNO_Audio=1"
	use debug && mycmakeargs="${mycmakeargs} -DDebug=1"
	use dedicated && mycmakeargs="${mycmakeargs} -DServer=1"
	use nls && mycmakeargs="${mycmakeargs} -DNLS=1"
	mycmakeargs="${mycmakeargs} -DCMAKE_INSTALL_PREFIX=${GAMES_PREFIX}
	-DCMAKE_DATA_PATH=${GAMES_DATADIR}
	-DCMAKE_LOCALE_PATH=${GAMES_DATADIR_BASE}/locale/
	-DCMAKE_DOC_PATH=${GAMES_DATADIR_BASE}/doc/
	-DCMAKE_ETC_PATH=${GAMES_SYSCONFDIR} -DLIB_INSTALL_DIR=$(games_get_libdir)"
	cmake-utils_src_configure
}

src_install() {
	cmake-utils_src_install
	#dosym ${GAMES_BINDIR}/${P} ${GAMES_BINDIR}/${PN}
	dosym ${GAMES_BINDIR}/${PN}-svn ${GAMES_BINDIR}/${PN}
	doicon data/${PN}.svg
	# we compile our desktop file
	cd "${WORKDIR}"/tuxanci_build
	domenu data/${PN}.desktop

	prepgamesdirs
}
