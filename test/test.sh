#!/usr/bin/env bash

# assume ci is always docker based
if command -v docker &> /dev/null || [[ "$1" == "ci" ]]; then
    CONTAINER_CMD="docker"
elif command -v podman &> /dev/null; then
    CONTAINER_CMD="podman"
elif command -v flatpak-spawn --host podman &> /dev/null; then
    CONTAINER_CMD="flatpak-spawn --host podman"
else 
    echo "could not find supported container engine like docker or podman"
    exit 1
fi

launch_systemd_container() {
  echo "launching systemd container named $1"

  $CONTAINER_CMD container rm "$1" --force --time 1 || true
  $CONTAINER_CMD kill "$1" || true

  # privileged is necessary for bwrap which is needed for flatpak; there might be a way around this but it is fine for now.
  $CONTAINER_CMD run -d --privileged --name "$1" "$1"
  sleep 1
  $CONTAINER_CMD ps

  echo "launched systemd container named $1"
}

run() {
  echo "running command (second argument) in container named $1"
  $CONTAINER_CMD exec "$1" bash -c "su - testuser -c '$2'"
}

$CONTAINER_CMD build -t easyeffects-systemd .

launch_systemd_container easyeffects-systemd


# nohup Xvfb $DISPLAY -screen 0 1280x1024x24 &
# run easyeffects-test "export DISPLAY=:10"

# since compiling is a pain should try dnf downgrade hacks
# can follow predictable ish urls (pkgver-1 could be pkgver-2 though)

# https://kojipkgs.fedoraproject.org/packages/pipewire/0.3.41/1.fc36/x86_64/

echo "starting to run commands like flatpak run in container"
#run easyeffects-systemd "xvfb-run flatpak repair --user"
#run easyeffects-systemd "dbus-run-session"
# todo not actually being run with custom pipewire atm, need make run
# use git describe --tags $(git rev-list --tags --max-count=1) to get the latest pipewire tag, testing master is silly.
# this is cached so nothing will actually be downloaded at runtime
run easyeffects-systemd "xvfb-run flatpak install --user com.github.wwmm.easyeffects//stable -y"
#run easyeffects-systemd "xvfb-run flatpak list --verbose --user -d"
# run easyeffects-systemd "xvfb-run flatpak update --verbose --user -y"
run easyeffects-systemd "xvfb-run flatpak list --user"

# run easyeffects-systemd "cd /home/testuser/.local/share/flatpak && ls"

# start easyeffects
run easyeffects-systemd "export G_MESSAGES_DEBUG=easyeffects && xvfb-run flatpak run --user com.github.wwmm.easyeffects//stable" &
sleep 5 # todo instead of sleep we should try something more robust. Is there a way to use dbus to poke at state of app?

run easyeffects-systemd "pgrep easyeffects && (echo EasyEffects is Running && exit 0) || (echo EasyEffects is not running so exiting && exit 1)"

# tests I want?

# everything in config.yml (advanced audio) and test.sh (startup)
