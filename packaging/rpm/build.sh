#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Build the Fedora/Bazzite RPM from the current git tree. Run it inside a Fedora
# container with the build dependencies installed (the release workflow does
# that); the result is copied to packaging/*.rpm, next to the Arch package.
#
# Like packaging/build.sh, the source tarball is created from HEAD, so only
# committed files are packaged and the version always matches the project.
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$here/../.." && pwd)"
cd "$here"

version="$(sed -n 's/^set(PROJECT_VERSION "\(.*\)")$/\1/p' "$root/CMakeLists.txt" | head -n1)"
if [ -z "$version" ]; then
    echo "Could not determine project version" >&2
    exit 1
fi

sed -i "s/^Version:.*/Version:        $version/" plasma-keyboard-custom.spec

rm -f "plasma-keyboard-custom-$version.tar.gz"
git -C "$root" archive --format=tar.gz \
    --prefix="plasma-keyboard-custom-$version/" HEAD \
    > "plasma-keyboard-custom-$version.tar.gz"

topdir="$(mktemp -d)"
trap 'rm -rf "$topdir"' EXIT

# debug_package is disabled the same way the Arch package disables it: the debug
# package only duplicates the symbols in every release asset.
rpmbuild -bb plasma-keyboard-custom.spec \
    --define "_topdir $topdir" \
    --define "_sourcedir $here" \
    --define "debug_package %{nil}"

rm -f "$here"/*.rpm
find "$topdir/RPMS" -name '*.rpm' -exec cp -v {} "$here/" \;
ls -l "$here"/*.rpm
