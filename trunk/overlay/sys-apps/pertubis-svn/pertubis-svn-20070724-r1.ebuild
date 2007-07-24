# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit subversion

ESVN_REPO_URI="svn://svn.berlios.de/pertubis/trunk/pertubis/"
ESVN_PROJECT="pertubis-svn"

ESVN_STORE_DIR="${DISTDIR}/svn-src"

DESCRIPTION="a graphical frontend for paludis using qt4 ( development version )"
HOMEPAGE="http://www.pertubis.berlios.de"

SRC_URI=""

KEYWORDS="~amd64 ~x86"
LICENSE="GPL-2"

SLOT="0"
IUSE=""

RDEPEND="dev-libs/pcre++
		>=x11-libs/qt-4.1.0"

DEPEND="${RDEPEND}
		>=dev-util/cmake-2.2.3"

S="${WORKDIR}"/pertubis

src_unpack() {
	ewarn
	ewarn "This is an Subversion snapshot, so no functionality is"
	ewarn "guaranteed!"
	subversion_fetch
	mkdir "${S}/build"
}

src_compile() {

	cd "${S}/build"
	local CMAKE_VARIABLES=""
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DCMAKE_SKIP_RPATH:BOOL=YES"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DQT_WRAP_CPP:BOOL=ON"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DCMAKE_VERBOSE_MAKEFILE:BOOL=NO"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DQT_WRAP_UI:BOOL=ON"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DQT_MOC_EXECUTABLE:FILEPATH=/usr/bin/moc"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DQT_UIC_EXECUTABLE:FILEPATH=/usr/bin/uic"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DQT_INCLUDE_DIR:PATH=/usr/include/qt4"
	CMAKE_VARIABLES="${CMAKE_VARIABLES} -DQT_QMAKE_EXECUTABLE:PATH=/usr/bin/qmake"
	cmake ${CMAKE_VARIABLES} .. || die "cmake configuration failed"
	emake -j1 || die "emake failed"
}

src_install() {

	cd "${S}/build"
	make DESTDIR=${D} install || die "make install failed"
}
