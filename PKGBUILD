# Maintainer: Bastian Engel <bastian.engel00 at gmail.com>

pkgname=vex-git
pkgver=v1.0.0.r0.abf98ec
pkgrel=1
pkgdesc="a simple ncurses-based hex editor"
arch=('x86_64' 'i686')
url="https://bengel.xyz"
license=('GPL')
groups=()
depends=('ncurses>=6.3-3')
makedepends=('git' 'meson')
provides=("vex")
conflicts=("vex")
source=("git+https://github.com/Baseng0815/vex.git")
md5sums=('SKIP')

pkgver() {
	git -C vex describe --long | sed 's/\([^-]*-\)g/r\1/;s/-/./g'
}

build() {
	meson --prefix=/usr --buildtype=release vex build
        meson compile -C build
}

package() {
	meson install -C build --destdir "$pkgdir"
}
