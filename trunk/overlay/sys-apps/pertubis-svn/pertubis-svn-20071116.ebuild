# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit subversion cmake-utils

ESVN_REPO_URI="svn://svn.berlios.de/pertubis/trunk/pertubis/"
ESVN_PROJECT="pertubis-svn"

ESVN_STORE_DIR="${DISTDIR}/svn-src"

DESCRIPTION="a graphical frontend for paludis using qt4 ( development version )"
HOMEPAGE="http://www.pertubis.berlios.de"

SRC_URI=""

KEYWORDS="~amd64 ~x86"
LICENSE="GPL-2"

SLOT="0"
IUSE="debug"

RDEPEND="dev-libs/pcre++
        >=x11-libs/qt-4.3.0"

DEPEND="${RDEPEND}
        >=dev-util/cmake-2.4.7"

S="${WORKDIR}"/pertubis

src_setup() {
    ewarn
    ewarn "This is an Subversion snapshot, so no functionality is"
    ewarn "guaranteed!"
    subversion_fetch
    # Enable debug via use flag switch
    if use debug; then
        mycmakeargs="${mycmakeargs} -DCMAKE_BUILD_TYPE=Debug"
    fi

    cmake_utils_configureout
}

src_compile() {

    cmake-utils_src_compile
}

src_install() {

    cmake-utils_src_install
}
