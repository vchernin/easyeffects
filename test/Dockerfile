FROM fedora:latest
# todo try follow https://github.com/flatpak/flatpak-github-actions/blob/master/Dockerfile
# use security=insecure to allow installing with flatpak https://github.com/flatpak/flatpak-github-actions/blob/0e15d5b395c454b32562e72ff4ccccd9cec78954/.github/workflows/docker.yml#L126C12-L135
# RUN pacman -Syu --noconfirm systemd pipewire pipewire-pulse

RUN dnf update -y

RUN dnf -y install systemd 'dnf-command(builddep)' util-linux flatpak xorg-x11-server-Xvfb git dbus-daemon
RUN dnf -y --allowerasing builddep pipewire
#RUN dnf -y install alsa-utils pulseaudio-utils
# ; dnf clean all

RUN useradd testuser -u 1000 -m -p ''

RUN usermod -aG audio testuser

WORKDIR /home/testuser

RUN sudo -H -u testuser git clone https://github.com/pipewire/pipewire
RUN sudo -H -u testuser bash -c 'cd /home/testuser/pipewire && meson builddir && ./pw-uninstalled.sh -b builddir && meson configure builddir -Dlibcamera=disabled && cd builddir && make' 
# maybe need --device /dev/snd to make run properly. or perhaps try no make in dockerfile

#RUN sudo -H -u testuser systemctl enable --user pipewire pipewire-pulse

#RUN dbus-daemon --system
#RUN sudo -H -u testuser dbus-daemon --session
RUN su -c "flatpak remote-add flathub --user https://flathub.org/repo/flathub.flatpakrepo" testuser
RUN su -c "flatpak install -y --user com.github.wwmm.easyeffects//stable --no-deploy" testuser
# can use flatpak-builder --user build-dir util/flatpak/com.github.wwmm.easyeffects.Devel.json --install-deps-from=flathub --install-deps-only
# to only install some deps, but misses audio extensions. Would then need to hardcode audio extensions
# but with stable we might be out of date anyhow.

CMD [ "/sbin/init" ]

