# Contributor: Wellington <wellingtonwallace@gmail.com>

pkgname=easyeffects-upstream
pkgver=pkgvernotupdated
pkgrel=1
pkgdesc='Audio Effects for PipeWire Applications'
arch=(x86_64)
url='https://github.com/wwmm/easyeffects'
license=('GPL3')
depends=('gtk4' 'libadwaita' 'glib2' 'pipewire-pulse' 'lilv' 'lv2' 'libsigc++-3.0' 'libsndfile' 'libsamplerate' 'zita-convolver' 
         'libebur128' 'rnnoise' 'rubberband' 'fftw' 'libbs2b' 'speexdsp' 'nlohmann-json' 'tbb' 'fmt')
makedepends=('meson' 'itstool' 'appstream-glib' 'git')
optdepends=('calf: limiter, exciter, bass enhancer and others'
            'lsp-plugins: equalizer, compressor, delay, loudness'
            'zam-plugins: maximizer'
            'mda.lv2: bass loudness'
            'yelp: in-app help')
source=('../easyeffects')
conflicts=(easyeffects)
provides=(easyeffects)
replaces=('pulseeffects')
sha512sums=('SKIP')

pkgver() {
  git describe --long | sed 's/^v//;s/\([^-]*-g\)/r\1/;s/-/./g'
}

build() {
  cd ..
  arch-meson . build

  ninja -C build
}

package() {
  cd ..
  DESTDIR="${pkgdir}" ninja install -C build
}
