# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils

DESCRIPTION="a graphical frontend for paludis using qt4"
HOMEPAGE="http://www.pertubis.berlios.org"

KEYWORDS="~amd64 ~x86"
LICENSE="BSD"
SLOT="0"
IUSE=""
RDEPEND="dev-libs/pcre++
		>=x11-libs/qt-4.1.0"

SRC_URI="http://www.pertubis.berlios.org/files/${P}.tar.bz2"

DEPEND="${RDEPEND}
		>=dev-util/cmake-2.2.3"

S="${WORKDIR}"/pertubis

src_unpack() {
	unpack ${A}
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
