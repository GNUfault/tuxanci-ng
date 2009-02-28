# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI="2"

EGIT_REPO_URI="git://repo.or.cz/tuxanci.git"
inherit cmake-utils games git

DESCRIPTION="Tuxanci is first tux shooter inspired by game Bulanci."
HOMEPAGE="http://www.tuxanci.org/"
#SRC_URI="http://download.${PN}.org/${P}.tar.bz2"
LICENSE="GPL-2"

SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="alsa debug dedicated nls opengl"
# alsa is used only when building client

RDEPEND="!dedicated? (
			media-libs/libsdl[X,opengl?]
			media-libs/sdl-ttf[X]
			media-libs/sdl-image[png]
			alsa? (
				media-libs/sdl-mixer[vorbis]
			)
		)
	dev-libs/zziplib[sdl]"
DEPEND="${RDEPEND}
	>=dev-util/cmake-2.6.0
	nls? ( sys-devel/gettext )"

src_configure() {
	local mycmakeargs="$(cmake-utils_use_with alsa AUDIO)
		$(cmake-utils_use_enable debug DEBUG)
		$(cmake-utils_use_with dedicated SERVER)
		$(cmake-utils_use_with nls NLS)
		$(cmake-utils_use_with opengl OPENGL)"
	mycmakeargs="${mycmakeargs}
		-DCMAKE_INSTALL_PREFIX=${GAMES_PREFIX}
		-DCMAKE_DATA_PATH=${GAMES_DATADIR}
		-DCMAKE_LOCALE_PATH=${GAMES_DATADIR_BASE}/locale/
		-DCMAKE_DOC_PATH=${GAMES_DATADIR_BASE}/doc/${PF}
		-DCMAKE_CONF_PATH=${GAMES_SYSCONFDIR}
		-DLIB_INSTALL_DIR=$(games_get_libdir)"

	cmake-utils_src_configure
}

src_install() {
	cmake-utils_src_install

	doicon data/${PN}.svg
	domenu data/${PN}.desktop

	prepgamesdirs
}
