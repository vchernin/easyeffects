This script is a runner of integration tests for easyeffects. It mainly checks for crashes found only at runtime. It does not (yet) attempt to any complex real world simulation, or interact with the gui.

It was designed around the idea of running as much as possible in a container, as most times you want to be running what CI runs and helping confirming the one source of truth, trying to keep away from "works on my machine".

That said the script does let you run with the host pipewire daemon, either with a flatpak or host build. We do not use a container here as we cannot maintain configurations for every distro with guarantees of correctnes, not to mention it is nearly impossible to guarantee the same packages are present in the container used for testing and on your host (meaning a container-based test for your issue may not reliably reproduce your issue). Be warned this will mess with your easyeffects instance, change its configuration and add dummy devices to your system pipewire daemon. It will however attempt to reverse the changes, but it is impossible to guarantee correctness especially if you try to actively modify the easyeffects instance yourself while the test is running.

provide either path to easyeffects.flatpak, or to easyeffects binary.
./test.sh easyeffects.flatpak --pipewire-tags=0.41,0.48,0.60,latest-fedora,latest-upstream (use git switch --detach)

When passing path to easyeffects binary --pipewire-tags is not supported.

latest-fedora uses the latest fedora release's pipewire version, latest-upstream is the latest upstream tag.

To test with a sanitized build (-Dsanitzers=true) simply build as usual and pass the path to the sanitized build flatpak bundle/binary. 

Big todo:

rewrite script in python for sane arg parsing

sanitizers todos:
don't bother with valgrind for memory leaks for now, but it probably wouldnt't be hard to change flatpak run and binary run to use valgrind.

Sanitizer:
Use article config:
ASAN, GOOD
UBSAN  GOOD
Extra flags from article GOOD

Uninitialized memory - memory saniziter (needs clang, and rebuild. Just use valgrind/memcheck)

thread sanitizer ? (can i enable this without rebuild?)
leak sanitizer ? (can i enable this without rebuild?) (can be enabled as part of addresssantizer https://www.jetbrains.com/help/clion/google-sanitizers.html#LSanChapter)
 
https://developers.redhat.com/blog/2021/05/05/memory-error-checking-in-c-and-c-comparing-sanitizers-and-valgrind#tldr

integration tests:

# manual script usage (non ci, flatpak):

### manual flatpak (without meson)
just run the script as is, and can pass options to use pipewire(s) in the container, meaning script needs to shove bundle into container. 0r can not pass options to use current local flatpak build with host pipewire daemon.

### flatpak-builder (no gnome builder):
this is impossible to do well with when flatpak-builder runs with meson test, since it would need to either need a functional xfvb-run to create a dummy display in the sandbox, as well as creating a dummy pipewire daemon. or you would need to allow arbitary commands with flatpak-spawn --host, but this is also not useful since we onlt want to test with the fresh build, how will the host find it?

### gnome builder with flatpak build:
to run appstream and desktop tests using unit tests feature need to be in this "from build pipeline" mode
since location as application is somewhat unknown. So we cant run them "as application".
not possible for same reason as flatpak-builder with unit tests feature, from build pipeline or from as application.

However can likely create custom command to run the script. to be pretty much be a wrapper around the test script. or maybes its best to have such logic in the script itself.

Same issues would exist with non flatpak easyffects builds, they are still stuck in gnome builder sandboxes. not going to worry much if running with gnome builder on the host, since that should be a trivial custom command.


# meson test (non ci, non flatpak):

debian host or fedora host and fedora toolbox should be identical, should have access to everything that host does, so piggyback on existing daemon.
there shouldnt be a tangible difference between "build pipeline" vs "as application" here

using with pipewire containers is not supported (meaning must pass no options), as we can't just shove the build in a fedora container in case this is a arch build. Not to mention which is what you'd care about since usually pipewire daemon and lib version matches on non flatpaks. We could however offer to build and run custom pipewire(s) on the host, but that is likely not very useful. 


# meson test (in our CI):

## flatpak:
in ci do matrix build and run the script for each pipewire version. should probably find way of caching, by building all pipewire versions in dockerfile and leaving them in their own directory. clean build cache otherwise it will take up too much room in github, probably.

## arch:
meson test must somehow launch working pipewire daemon. It is already in a docker image though, so it may not be possible to test this without reworking the image permissions. Verify the flatpak approach actually works before doing this.

could do similar thing for debian maybe have network access for meson test.

# meson test (in distro CI):
no one will be running these, so pretty much all N/A.

## flatpak (flathub):
not worth worrying about given nature of flatpak reproducibility.
not to mention they dont even let us to run tests due to --sandbox in flatpak-builder

## arch:
likely no network access for meson test, so no cloning pw daemon.  surely no way i have access to working daemon.

## debian:
no network access for meson test, so no cloning pw daemon. surely no way i have access to working daemon.