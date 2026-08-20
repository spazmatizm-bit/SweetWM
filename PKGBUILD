# Maintainer: spazmatizm <spazmatizm7@gmail.com>
pkgname=sweetwm
pkgver=1.0.0
pkgrel=1
pkgdesc="SweetWM - A custom X11 window manager with compositing, Qt menu, and full desktop utilities"
arch=('x86_64')
url="https://github.com/spazmatizm-bit/SweetWM"
license=('GPL3')
depends=(
    'libx11'
    'libxrender'
    'libxcomposite'
    'libxdamage'
    'libxfixes'
    'qt5-base'
    'kitty'
    'feh'
    'xorg-xrandr'
)
makedepends=('git' 'gcc' 'make' 'qt5-tools' 'pkg-config')
conflicts=('sweetwm-git')
provides=('sweetwm')
source=(
    "$pkgname::git+$url.git"
    "sweetwm.desktop"
)
sha256sums=('SKIP' 'SKIP')

pkgver() {
  cd "$srcdir/$pkgname"
  git describe --long --tags | sed 's/\(.*\)-g.*/\1/;s/^v//' || echo "r$(git rev-list --count HEAD).$(git rev-parse --short HEAD)"
}

build() {
  cd "$srcdir/$pkgname"
  make
  if [ -f src/qtmenu/menu.pro ]; then
    cd src/qtmenu
    qmake menu.pro
    make
  fi
}

package() {
  cd "$srcdir/$pkgname"
  install -Dm755 sweetwm "$pkgdir/usr/bin/sweetwm"
  install -Dm755 sweetwm-tui "$pkgdir/usr/bin/sweetwm-tui"
  [ -f src/qtmenu/sweetmenu ] && install -Dm755 src/qtmenu/sweetmenu "$pkgdir/usr/bin/sweetmenu"
  install -Dm755 rebuild_and_test.sh "$pkgdir/usr/share/sweetwm/rebuild_and_test.sh"
  install -Dm755 test_xephyr.sh "$pkgdir/usr/share/sweetwm/test_xephyr.sh"
  install -Dm755 test_apps.sh "$pkgdir/usr/share/sweetwm/test_apps.sh"
  install -d "$pkgdir/usr/share/sweetwm/pictures"
  install -Dm644 userfiles/pictures/wallpaper1.png "$pkgdir/usr/share/sweetwm/pictures/wallpaper1.png"
  install -Dm644 userfiles/pictures/wallpaper2.png "$pkgdir/usr/share/sweetwm/pictures/wallpaper2.png"
  install -Dm644 "$srcdir/sweetwm.desktop" "$pkgdir/usr/share/xsessions/sweetwm.desktop"
}
