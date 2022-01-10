# EasyEffects

[![CircleCI](https://circleci.com/gh/wwmm/easyeffects.svg?style=shield)](https://circleci.com/gh/wwmm/easyeffects)
[![Donate](https://liberapay.com/assets/widgets/donate.svg)](https://liberapay.com/wwmm/donate)

Audio effects for PipeWire applications.

This application was formerly known as PulseEffects, but it was renamed to EasyEffects after it started to use GTK4 and GStreamer usage was replaced by native PipeWire filters.

![GitHub Light](images/easyeffects-light-screenshot-1.png#gh-light-mode-only)
![GitHub Light](images/easyeffects-light-screenshot-2.png#gh-light-mode-only)
![GitHub Light](images/easyeffects-light-screenshot-3.png#gh-light-mode-only)

![GitHub Dark](images/easyeffects-dark-screenshot-1.png#gh-dark-mode-only)
![GitHub Dark](images/easyeffects-dark-screenshot-2.png#gh-dark-mode-only)
![GitHub Dark](images/easyeffects-dark-screenshot-3.png#gh-dark-mode-only)

## Effects available

Commit notes (todo remove):
- Reorganized 
- Did not remove significant information, simply reorgnized it into a table.
- Remove Expander (is not in the app and not mentioned anywhere else)
- Todo, resolve the todos
- Figure out why to hide description collumn somehow.


EasyEffects includes many audio plugins. See the Manual provided in the app (hamburger menu) with detailed instructions on how to use each plugin.

| Plugin         | Description     | Provided by |
|--------------|-----------|-----------|
| Auto Gain | Changes the audio volume to a perceived loudness target that can be customized by the user. | [libebur128](https://github.com/jiixyj/libebur128)        |
| Bass Enhancer      | Produce very low sound that is not present in the original signal. |  [Calf Studio Gear](https://calf-studio-gear.org/) |
| Bass Loudness      | Allows the bass level to be adjusted measuring the "equal-loudness contour".  | MDA Plugins |
| Compressor      | Reduces the dynamic range or, in other words, the difference in level between the quietest and the loudest parts of an audio signal.  | [Linux Studio Plugins](http://lsp-plug.in/?page=home) |
| Convolver      | Creates a simulation of an audio environment using a pre-recorded audio sample of the impulse response of the space being modeled.  | [zita-convolver](https://kokkinizita.linuxaudio.org/linuxaudio/) |
| Crossfeed      | Improves headphone listening of stereo audio records by mixing the left and right channel in a way that simulates a stereo speaker setup.  | [libbs2b](https://sourceforge.net/projects/bs2b/files/libbs2b/) |
| Crystalizer      | Adds a little of dynamic range to songs that were overly compressed.  | N/A |
| De-esser      | Dynamically reduce high frequencies, commonly used to reduce "sssss" and "shhhh" in vocal tracks.  | [Calf Studio Gear](https://calf-studio-gear.org/) |
| Echo Canceller      |  Improve voice quality by preventing Echo from being created or removing it after it has been added to the source signal. | [speexdsp](https://www.speex.org/) |
| Equalizer      | Allows adjusting the volume of different frequency bands within an audio signal. | [Linux Studio Plugins](http://lsp-plug.in/?page=home) |
| Exciter      | Produces high sound that is not present in the original signal.  | [Calf Studio Gear](https://calf-studio-gear.org/) |
| Filter | Used to amplify (boost), pass or attenuate (cut) defined parts of a frequency spectrum.  | [Calf Studio Gear](https://calf-studio-gear.org/) |
| Gate      | Attenuates signals that register below a Threshold, often used to reduce disturbing noise between useful signals.  |  [Calf Studio Gear](https://calf-studio-gear.org/) |
| Limiter      | Type of downward Compressor which does not allow the signal to overtake a predetermined Threshold.  | [Linux Studio Plugins](http://lsp-plug.in/?page=home) |
| Loudness      | Applies the "equal-loudness contour" corrections to the input signal.  | [Linux Studio Plugins](http://lsp-plug.in/?page=home) |
| Maximizer      |  Type of Limiter that does not only prevent the signal to exceed a specified target level, but also adjusts the average Loudness of the audio track. | [ZamAudio plugins](http://www.zamaudio.com/) |
| Multiband Compressor      | An alternative advanced Multiband Compressor, see also "Compressor".  | [Linux Studio plugins](http://lsp-plug.in/?page=home) |
| Multiband Gate      | An alternative advanced Multiband Gate, see also "Gate". | [Calf Studio Gear](https://calf-studio-gear.org/) |
| Noise Reduction      | Used to attenuate the disturbing noise from a signal, such as background noise in a voice call.  | [RNNoise](https://github.com/xiph/rnnoise) |
| Pitch      | Used to raise or lower the original pitch of a sound.  | [Rubber Band](https://www.breakfastquay.com/rubberband/) |
| Reverberation      | Simulates the phenomenon of persistence of sound after the source has been stopped, as a result of multiple reflections of the waves over objects within a closed surface. | [Calf Studio Gear](https://calf-studio-gear.org/) |
| Stereo Tools      | Provides some handy utilities to manage Stereo streams handling Left and Right channels, in conjunction with Mid and Side signals.  | [Calf Studio Gear](https://calf-studio-gear.org/) |

The user has full control over the effects order. Just use the up/down arrows
next to the effect labels on the left side.

Some packages do not provide all plugin packages by default. To be able to use all of the effects, you must have the following installed on your system:
Todo figure out which ones I actually need to list here.

Note: the Flathub package includes support for all available plugins by default.


- [Linux Studio plugins](http://lsp-plug.in/?page=home). Version 1.1.24 or higher.
- [Calf Studio plugins](https://calf-studio-gear.org/). Version 0.90.1 or higher.
- [libebur128](https://github.com/jiixyj/libebur128). For Auto Gain.
- [ZamAudio plugins](http://www.zamaudio.com/). For Maximizer.
- [zita-convolver](https://kokkinizita.linuxaudio.org/linuxaudio/). For Convolver.
- [rubberband](https://www.breakfastquay.com/rubberband/). For Pitch Shift.
- [RNNoise](https://github.com/xiph/rnnoise). For Noise Reduction.


todo these can probably be removed or moved (they are hard runtime requirements?):

- [libsamplerate](http://www.mega-nerd.com/SRC/index.html)
- [libsndfile](http://www.mega-nerd.com/libsndfile/)
- [libbs2b](https://sourceforge.net/projects/bs2b/files/libbs2b/)
- [fftw](https://fftw.org/)
- [speexdsp](https://www.speex.org/)
- [nlohmann json](https://github.com/nlohmann/json)
- [tbb](https://www.threadingbuildingblocks.org)

## Donate

You can help me to keep developing EasyEffects through donations. Any amount will be greatly appreciated :-)

<table>
  <tr>
    <td><a href="https://liberapay.com/wwmm/">https://liberapay.com/wwmm</a></td>
    <td><a href="https://liberapay.com/wwmm/donate"><img alt="Donate using Liberapay" src="https://liberapay.com/assets/widgets/donate.svg"></a></td>
  </tr>
  <tr>
    <td>https://www.patreon.com/wellingtonwallace?fan_landing=true</td>
    <td>Patreon</td>
  </tr>
  <tr>
    <td>https://www.paypal.com/donate?hosted_button_id=RK723F4EKH2UE</td>
    <td>PayPal</td>
  </tr>
</table>

## Changelog

- For information about changes between versions, take a look at our
  [changelog](https://github.com/wwmm/easyeffects/blob/master/CHANGELOG.md)

## Installation

### Distribution-specific packages

These Linux distributions have EasyEffects packages:

- [Arch Linux](https://www.archlinux.org/packages/community/x86_64/easyeffects/) / [AUR Git version](https://aur.archlinux.org/packages/easyeffects-git/)
- [openSUSE](https://software.opensuse.org/package/easyeffects)
- [NixOS](https://search.nixos.org/packages?channel=unstable&show=easyeffects&query=easyeffects)
- [Gentoo](https://packages.gentoo.org/packages/media-sound/easyeffects)
- [Fedora](https://src.fedoraproject.org/rpms/easyeffects)

<!--
- [Void Linux](https://github.com/void-linux/void-packages/blob/master/srcpkgs/easyeffects/template)
- [ROSA](https://abf.io/import/easyeffects/)
- [ALT Linux](https://packages.altlinux.org/Sisyphus/srpms/easyeffects/)
- [Fedora GNU/Linux](https://apps.fedoraproject.org/packages/easyeffects)
- [CRUX](https://crux.nu/portdb/?a=search&q=easyeffects)
- [Debian (buster)](https://packages.debian.org/buster-backports/easyeffects) / [Debian (bullseye)](https://packages.debian.org/bullseye/easyeffects) -->

<!-- These are community maintained repositories of distribution packages. You can
find more information about these in the
[wiki](https://github.com/wwmm/easyeffects/wiki/Package-Repositories#package-repositories).

- [Ubuntu and Debian](https://github.com/wwmm/easyeffects/wiki/Package-Repositories#debian--ubuntu) -->

### Flatpak

Alternatively, click here to install via Flatpak:

<a href='https://flathub.org/apps/details/com.github.wwmm.easyeffects'><img width='240' alt='Download EasyEffects on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/></a>

Flatpak packages support most Linux distributions and are sandboxed. The EasyEffects Flatpak package also includes all available plugins.

If your distribution does not yet include packages required to build EasyEffects, Flatpak is a convenient option.

#### Nightly Flatpak

A nightly Flatpak package is also available, to provide the latest changes. See the [installation instructions](https://github.com/wwmm/easyeffects/wiki/Package-Repositories#nightly-flatpak).

### Installing from source

To install from source, see the wiki's [Installing from Source](https://github.com/wwmm/easyeffects/wiki/Installation-from-Source) article for detailed instructions.

## Documentation

EasyEffects documentation can be read inside the GUI if the
[yelp](https://gitlab.gnome.org/GNOME/yelp) package is installed.

## Frequently asked questions

Take a look at our [FAQ](https://github.com/wwmm/easyeffects/wiki/FAQ) to see
if you are facing a known issue.

## Command-line options

See the wiki: [Command Line Options](https://github.com/wwmm/easyeffects/wiki/Command-Line-Options)

## Community presets

See the wiki: [Community presets](https://github.com/wwmm/easyeffects/wiki/Community-presets)

This page also has instructions for converting PulseEffects presets to EasyEffects presets.

## Reporting bugs

See the wiki: [Reporting Bugs](https://github.com/wwmm/easyeffects/wiki/Reporting-bugs)

## Translating EasyEffects

See the wiki's [Translating EasyEffects](https://github.com/wwmm/easyeffects/wiki/Translating-EasyEffects) article for detailed instructions.

## License

EasyEffects is licensed under GNU General Public License version 3. See the [LICENSE file](https://github.com/wwmm/easyeffects/blob/master/LICENSE.md).
