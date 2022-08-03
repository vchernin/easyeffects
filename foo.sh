CWD=$(pwd)
                
        podman run --rm --privileged \
          -v $HOME:$HOME:rslave \
          -v $CWD:$CWD:rslave \
          -w $CWD \
          -e GIT_AUTHOR_NAME="EasyEffects Bot" \
          -e GIT_COMMITTER_NAME="EasyEffects Bot" \
          -e GIT_AUTHOR_EMAIL="110548574+easyeffects-bot@users.noreply.github.com" \
          -e GIT_COMMITTER_EMAIL="110548574+easyeffects-bot@users.noreply.github.com" \
          -e EMAIL="110548574+easyeffects-bot@users.noreply.github.com" \
          -e GITHUB_TOKEN="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"  \
          ghcr.io/flathub/flatpak-external-data-checker:latest \
          --always-fork --update util/flatpak/com.github.wwmm.easyeffects.json