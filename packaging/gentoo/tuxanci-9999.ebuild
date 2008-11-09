# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

inherit cmake-utils git games

DESCRIPTION="Tuxanci is first tux shooter inspired by game Bulanci."
HOMEPAGE="http://www.tuxanci.org/"
EGIT_REPO_URI="git://repo.or.cz/tuxanci.git"
#SRC_URI="http://download.${PN}.org/${P}.tar.bz2"
LICENSE="GPL-2"

SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="alsa debug dedicated nls opengl"
# alsa is used only when building client

RDEPEND="!dedicated? (
			>=media-libs/libsdl-1.2.10[X]
			>=media-libs/sdl-ttf-2.0.7[X]
			>=media-libs/sdl-image-1.2.6-r1[png]
			alsa? (
				>=media-libs/sdl-mixer-1.2.7[vorbis]
			)
			opengl? (
				media-libs/libsdl[opengl?]
				virtual/opengl
			)
		)
	dev-libs/zziplib[sdl]"
DEPEND=">=dev-util/cmake-2.6.0
	nls? ( sys-devel/gettext )"

S="${WORKDIR}"/"${PN}"

src_configure() {
	local mycmakeargs
	use alsa || mycmakeargs="${mycmakeargs} -DNO_Audio=1"
	use debug && mycmakeargs="${mycmakeargs} -DDebug=1"
	use dedicated && mycmakeargs="${mycmakeargs} -DServer=1"
	use nls || mycmakeargs="${mycmakeargs} -DNO_NLS=1"
	use opengl || mycmakeargs="${mycmakeargs} -DNO_OPENGL=1"
	mycmakeargs="${mycmakeargs} -DCMAKE_INSTALL_PREFIX=${GAMES_PREFIX}
		-DCMAKE_DATA_PATH=${GAMES_DATADIR}
		-DCMAKE_LOCALE_PATH=${GAMES_DATADIR_BASE}/locale/
		-DCMAKE_DOC_PATH=${GAMES_DATADIR_BASE}/doc/${PF}
		-DCMAKE_ETC_PATH=${GAMES_SYSCONFDIR} -DLIB_INSTALL_DIR=$(games_get_libdir)"

	cmake-utils_src_configure
}

src_install() {
	local MY_PN
	use dedicated && MY_PN=${PN}-server || MY_PN=${PN}

	cmake-utils_src_install
	#dosym ${GAMES_BINDIR}/${P} ${GAMES_BINDIR}/${PN}
	dosym ${GAMES_BINDIR}/${MY_PN}-dev ${GAMES_BINDIR}/${MY_PN}
	doicon data/${PN}.svg
	# we compile our desktop file
	cd "${WORKDIR}"/tuxanci_build
	domenu data/${PN}.desktop

	prepgamesdirs
}
