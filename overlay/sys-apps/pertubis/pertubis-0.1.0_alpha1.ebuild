# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

# inherit subversion cmake-utils flag-o-matic
inherit cmake-utils flag-o-matic

DESCRIPTION="a graphical frontend for paludis using qt4 ( development version )"
HOMEPAGE="http://www.pertubis.berlios.de"

SRC_URI="http://download.berlios.de/pertubis/pertubis-0.1.0_alpha1.tar.bz2 http://download2.berlios.de/pertubis/pertubis-0.1.0_alpha1.tar.bz2"

KEYWORDS="~x86 ~amd64"
LICENSE="GPL-2"

SLOT="0"
IUSE="debug doc"

RDEPEND="=sys-apps/paludis-scm
	>=x11-libs/qt-4.3.0"

DEPEND="=sys-apps/paludis-scm
	>=x11-libs/qt-4.3.0
	>=dev-util/cmake-2.4.7
	doc? ( >=app-doc/doxygen-1.5.3 ) "

src_unpack() {
	unpack ${A}
}

src_compile() {

	replace-flags -Os -O2
	replace-flags -O3 -O2

	if use doc; then
		mycmakeargs="${mycmakeargs} -DPERTUBIS_BUILD_DOCS=ON -DPERTUBIS_DOC_PATH=/usr/share/doc/${PF}"
	fi

	cmake-utils_src_compile || die "cmake-utils_src_compile failed"
}

src_install() {

	cmake-utils_src_install || die "install failed"
	dodoc AUTHORS README TODO || die "doc install failed"
}
