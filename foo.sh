GIT_USER_NAME=$(git config user.name)
GIT_USER_EMAIL=$(git config user.email)

CWD=$(pwd)

podman run --rm --privileged \
    -v $HOME:$HOME:rslave \
    -v $CWD:$CWD:rslave \
    -w $CWD \
    -e GIT_AUTHOR_NAME="bla" \
    -e GIT_COMMITTER_NAME="foo" \
    -e GIT_AUTHOR_EMAIL="foo@example.com" \
    -e GIT_COMMITTER_EMAIL="foo@example.com" \
    -e GITHUB_TOKEN="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" \
    -it ghcr.io/vchernin/flatpak-external-data-checker:latest \
    --always-fork --update util/flatpak/com.github.wwmm.easyeffects.json
