#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
#
# Publishes the landing page from site/ into the root of the gh-pages branch.
#
# The branch also carries the pacman repository in repo/x86_64, which this
# script never touches: the landing files are copied next to it and only added,
# so a release that publishes a package at the same time cannot conflict with
# the page (deploy-repo.yml rewrites repo/ only).
#
# Usage:
#   tools/deploy-site.sh            prepare the worktree and show the diff
#   tools/deploy-site.sh --push     also commit and push to origin/gh-pages
#
# Environment: WORKTREE (default .dsh/tmp/gh-pages-worktree).

set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(dirname "$script_dir")
site_dir=$repo_root/site
worktree=${WORKTREE:-$repo_root/.dsh/tmp/gh-pages-worktree}
push=0

while [ $# -gt 0 ]; do
    case "$1" in
        --push) push=1; shift ;;
        --worktree) worktree=$2; shift 2 ;;
        -h|--help) sed -n '2,18p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "error: unknown option $1" >&2; exit 1 ;;
    esac
done

[ -f "$site_dir/index.html" ] || { echo "error: $site_dir/index.html is missing" >&2; exit 1; }

cd "$repo_root"

# A shallow worktree of the published branch: the page is built from scratch
# every time, so nothing stale can survive in the root of gh-pages.
if git -C "$worktree" rev-parse --git-dir >/dev/null 2>&1; then
    git -C "$worktree" fetch origin gh-pages --depth=1
    git -C "$worktree" reset --hard FETCH_HEAD
else
    git fetch origin gh-pages --depth=1
    mkdir -p "$(dirname "$worktree")"
    git worktree add --detach "$worktree" FETCH_HEAD
fi

# Copy the page into the branch root. repo/ (the pacman repository) is excluded
# from the copy and from --delete, so it is neither read nor removed here;
# --delete removes stale landing files only. The same exclusions and the guard
# below live in .github/workflows/deploy-site.yml.
rsync -a --delete \
    --exclude '/repo/' \
    --exclude '/.git' \
    --exclude '/CNAME' \
    "$site_dir/" "$worktree/"

echo "--- files that will be published ---"
git -C "$worktree" add -A
git -C "$worktree" status --short

# Refuse to publish anything that touches the package repository.
if [ -n "$(git -C "$worktree" status --porcelain -- repo/)" ]; then
    echo "error: this deployment would change repo/, refusing to commit" >&2
    git -C "$worktree" status --porcelain -- repo/ >&2
    exit 1
fi

if [ "$push" -eq 0 ]; then
    cat <<EOF

Prepared in $worktree (nothing was committed).
Review the page:  xdg-open $site_dir/index.html
Publish it:       tools/deploy-site.sh --push
EOF
    exit 0
fi

git -C "$worktree" commit -m "Add the project landing page"
GIT_SSH_COMMAND='ssh -F /dev/null' git -C "$worktree" push origin HEAD:gh-pages
echo "published to gh-pages"
