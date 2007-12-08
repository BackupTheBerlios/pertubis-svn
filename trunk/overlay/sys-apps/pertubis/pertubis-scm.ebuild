# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

# inherit subversion cmake-utils flag-o-matic
inherit subversion cmake-utils flag-o-matic

ESVN_REPO_URI="svn://svn.berlios.de/pertubis/trunk/pertubis/"
KDE_COMMAND=""
GNOME_COMMAND=""

DESCRIPTION="a graphical frontend for paludis using qt4 ( development version )"
HOMEPAGE="http://www.pertubis.berlios.de"

SRC_URI=""

KEYWORDS="~x86 ~amd64"
LICENSE="GPL-2"

SLOT="0"
IUSE="debug doc gnome kde tests"

# TODO: kde and gnome should be exclusive or, or if both are set, prefer kde?/gnome?
# the su tool
RDEPEND=">=sys-apps/paludis-0.26.0_alpha4
	>=x11-libs/qt-4.3.0
	kde? ( ( >=kde-base/kdesu-3.5.5 ) || ( >=kde-base/kdebase-3.5.5 ) )
	gnome? ( >=x11-libs/gksu-1.9.1 )"

DEPEND=">=sys-apps/paludis-0.26.0_alpha4
	>=x11-libs/qt-4.3.0
	>=dev-util/cmake-2.4.7
	doc? ( >=app-doc/doxygen-1.5.3 ) "

src_setup() {
	subversion_fetch
}

src_compile() {

	einfo "This is an Subversion snapshot, so no functionality is guaranteed!"

	replace-flags -Os -O2
	replace-flags -O3 -O2
	if use debug; then
        mycmakeargs="${mycmakeargs} -DCMAKE_BUILD_TYPE=debug"
	fi

	if use tests; then
		mycmakeargs="${mycmakeargs} -DPERTUBIS_BUILD_TESTS=ON"
	fi

	if use doc; then
        mycmakeargs="${mycmakeargs} -DPERTUBIS_BUILD_DOCS=ON -DPERTUBIS_DOC_PATH=/usr/share/doc/${PF}"
    fi

    if use kde; then
        mycmakeargs="${mycmakeargs} -DPERTUBIS_SU_TOOL='kde"
    elif use gnome; then
        mycmakeargs="${mycmakeargs} -DPERTUBIS_SU_TOOL='gnome"
    fi

	# mycmakeargs should be ="-DCMAKE_BUILD_TYPE=Debug -DPERTUBIS_SU_TOOL=kdesu -DPERTUBIS_BUILD_TESTS=1"
	# but the ebuild configures cmake with e.g. -DCMAKE_BUILD_TYPE=debug (see cmake . ${WORKDIR}
	einfo "mycmakeargs ${mycmakeargs}"

 	cmake-utils_src_compile || die "cmake-utils_src_compile failed"
}

src_test() {

	cmake-utils_src_test || die "tests failed"
}

src_install() {

	cmake-utils_src_install || die "install failed"
	dodoc AUTHORS README TODO || die "doc install failed"
}
