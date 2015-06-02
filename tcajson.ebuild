# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=4

inherit eutils

DESCRIPTION="The 'tcajson' C++ JSON Library"
HOMEPAGE=""
SRC_URI="http://charltontechnology.net/gentoo/amd64/distfiles/${P}.tar.gz"

LICENSE="lgpl"
SLOT="0"
KEYWORDS="x86 amd64"
IUSE=""

DEPEND=""
RDEPEND="${DEPEND}"

src_compile() {
	emake solib || die "Make failed"
}

src_install() {
	insinto /usr/lib
	dolib.so lib/tcajson.so.${PVR} || die "File copy failed"
	dolib.so lib/tcajson.so || die "File copy failed"
	insinto /usr/include/tcajson
	doins include/* || die "File copy failed"
}

