<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard (custom fork)

![Plasma Keyboard (custom) with the optional F1–F12 row and the gamepad glyphs on the mapped keys](docs/screenshots/keyboard-en.png)

> **This branch/repository is `plasma-keyboard-custom`**, a fork of
> [KDE plasma-keyboard](https://invent.kde.org/plasma/plasma-keyboard) with extra functionality for
> handheld / gamepad-driven use (MSI Claw, CachyOS + KDE Plasma 6 Wayland). It installs **next to**
> the official package and does not replace it — see
> [plasma-keyboard-custom (fork)](#plasma-keyboard-custom-fork) below.

The plasma-keyboard is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html) designed to integrate in Plasma.

It wraps Qt Virtual Keyboard in a window, and uses the input-method-v1 Wayland protocol to communicate with the compositor to function as an input method.

## plasma-keyboard-custom (fork)

### Install the latest release

The quickest way is the [install script](install.sh): it takes the newest release, downloads the
package, checks it against the published `SHA256SUMS`, installs it with `pacman -U` and restarts the
keyboard, so no pacman repository has to be configured first (it only needs `curl` and `pacman`, since
the packages are built for Arch-based systems):

```sh
curl -fsSL https://raw.githubusercontent.com/mops1k/plasma-keyboard-custom/master/install.sh | sh
```

Options are passed through `sh -s --`:

| Option | Meaning |
| --- | --- |
| `--tag <tag>` | install that release instead of the latest one |
| `--overwrite` | let pacman replace files it does not track (e.g. after a manual `cmake --install`) |
| `--no-restart` | do not restart a running keyboard |
| `--dry-run` | download and verify the package, install nothing |
| `-h`, `--help` | usage |

Run it again for later updates, or add the [pacman repository](#install-from-the-pacman-repository)
below and let `sudo pacman -Sy plasma-keyboard-custom` do that.

By hand, the same thing: download the newest `plasma-keyboard-custom-*-x86_64.pkg.tar.zst` from the
[Releases page](https://github.com/mops1k/plasma-keyboard-custom/releases/latest):

```sh
url=$(curl -fsSL https://api.github.com/repos/mops1k/plasma-keyboard-custom/releases/latest \
      | grep -o 'https://[^"]*/plasma-keyboard-custom-[0-9][^"]*\.pkg\.tar\.zst' | head -n1)
curl -fsSLo /tmp/plasma-keyboard-custom.pkg.tar.zst "$url"
```

Optionally check it against the published checksums:

```sh
curl -fsSLo /tmp/SHA256SUMS "${url%/*}/SHA256SUMS"
(cd /tmp && sha256sum -c --ignore-missing SHA256SUMS)
```

Then install it:

```sh
sudo pacman -U /tmp/plasma-keyboard-custom.pkg.tar.zst
```

**To update**, run the same command (or the install script) again — the package version (and `pkgrel`)
grows with every release, so pacman upgrades the installed package in place. The keyboard restarts
itself and picks up the new binary; on a release without that, restart it by hand (or log out and back
in):

```sh
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod ''
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod \
    '/usr/share/applications/org.kde.plasma.keyboard.custom.desktop'
```

Then pick **plasma-keyboard-custom** in **System Settings → Virtual Keyboard**; its own settings are under
**System Settings → Plasma Keyboard (custom)**. It installs next to the official `plasma-keyboard` package.

To build the package yourself: `bash packaging/build.sh`.

### Install from the pacman repository

Every published release is also pushed into a small pacman repository on the `gh-pages` branch
(`repo/x86_64/`), so the package can be installed and upgraded with pacman instead of being
downloaded by hand. Add the repository once to `/etc/pacman.conf` — the packages are not signed,
hence `SigLevel`:

```ini
[plasma-keyboard-custom]
SigLevel = Optional TrustAll
Server = https://mops1k.github.io/plasma-keyboard-custom/repo/$arch
```

Then install it, and simply run `pacman -Sy plasma-keyboard-custom` again for every new release:

```sh
sudo pacman -Sy plasma-keyboard-custom
```

Restart the keyboard after an update exactly as described above. The repository always publishes the
newest package, but keeps the previous versions around (5 of them) in `repo/x86_64/`, so rolling back
is one command — pacman cannot install an older version from a repository database, an explicit
package is the way:

```sh
sudo pacman -U https://mops1k.github.io/plasma-keyboard-custom/repo/x86_64/plasma-keyboard-custom-<version>-x86_64.pkg.tar.zst
```

Hold the package back (`IgnorePkg = plasma-keyboard-custom` in `/etc/pacman.conf`, or
`pacman -Syu --ignore plasma-keyboard-custom`) if a later upgrade should not undo the rollback.
Older versions also stay attached to their GitHub releases and in `/var/cache/pacman/pkg/` until
`pacman -Sc`.

The repository is maintained automatically by
[`.github/workflows/deploy-repo.yml`](.github/workflows/deploy-repo.yml) whenever a release is
published; the workflow can also be re-run by hand from the Actions tab — with a release tag it
republishes that release, without one it re-indexes every published release (useful after a failed
run, or if the branch was lost).

### About

This is a **fork of [KDE plasma-keyboard](https://invent.kde.org/plasma/plasma-keyboard)** (based on the 6.7.90 sources) with
extra functionality for handheld / gamepad-driven use, primarily tested on an MSI Claw running CachyOS + KDE Plasma 6 Wayland.

It installs next to the official package and does not replace it: everything is renamed
(`plasma-keyboard-custom` binary, `org.kde.plasma.keyboard.custom*` QML modules, `kcm_plasmakeyboardcustom`, layouts in
`share/plasma/keyboard-custom`, style `PlasmaBreeze`), so both the stock and the custom keyboard show up under
**System Settings → Virtual Keyboard** and can be selected there.

### Screenshots

The keyboard (the F1–F12 row is optional, the gamepad glyphs are drawn on the mapped keys):

| English | Russian (PC-style layout) |
| --- | --- |
| ![English keyboard with the F1–F12 row](docs/screenshots/keyboard-en.png) | ![Russian keyboard](docs/screenshots/keyboard-ru.png) |

The settings page in System Settings:

| *Opening* | *Appearance* | *Typing* |
| --- | --- | --- |
| ![Opening settings](docs/screenshots/settings-opening.png) | ![Appearance settings](docs/screenshots/settings-appearance.png) | ![Typing settings](docs/screenshots/settings-typing.png) |

### What is different from upstream

- **Gamepad support** via InputPlumber's system D-Bus target (`org.shadowblip.Input.DBusDevice`):
  - D-pad / left stick navigate, **A** selects, **B** closes, **X** backspace (holding it keeps deleting, like a key on a
    hardware keyboard), **Y** space, **LT** shift, **RT** enter, **LB** symbols, **RB** switches the layout.
  - Button glyphs are shown directly on the mapped keys (X, RT, LT, LB, RB, Y, B) plus an **A** badge on the focused key.
  - While the keyboard is visible the gamepad is intercepted (InputPlumber `InterceptMode = GAMEPAD_ONLY`) so input
    does not leak into the game or Steam's mapping; the previous mode is restored on hide/exit.
- **Latching Ctrl / Alt / Shift**: tapping a modifier latches it (shown by a lighter key background), it applies to the
  next key and then clears; tapping again unlatches. Works the same for touch, key navigation and the gamepad. Shift
  takes part in combinations (e.g. `Ctrl+Shift+key`).
- **Layout key**: a single tap switches to the next layout, a long press opens the language popup (plus Settings).
- **Rewritten layouts** in a PC style: all letter layouts (`fallback`/English, `ru_RU`, `lv_LV` plus the other Latin,
  Cyrillic and Greek ones) use the same rows as `ru_RU` — `Esc` and a hide-keyboard key, `Ctrl`/`Alt`, a numeric row,
  physical inverted-T arrow cluster, `Del`/`Shift`/`&123` sized like `Tab`, no right Shift; the Latvian layout keeps its
  long-press diacritics. Multi-mode layouts (Japanese, Korean, Chinese, Thai, Arabic, Hebrew) are left as upstream.
- **Breeze style**: installed as `PlasmaBreeze` (so it is not shadowed by the system one), configurable keyboard height,
  bold function/modifier keys, monochrome globe for the language key, capitalized language name on the space key.
- **Working sound feedback**: upstream declares its CMake option as `PLASMA_KEYBOARD_SOUNDS_ENABLED` while everything
  else looks for `PLASMA_KEYBOARD_SOUND_ENABLED`, so the key click was never compiled in and the setting was forced
  off. The option is fixed here, the click plays at full volume and the bundled GPLv3 sound (from Qt Virtual Keyboard)
  is amplified, because the upstream asset peaks at only −20.8 dB.
- **Open on long press**: instead of popping up as soon as a text field is focused, the keyboard can wait for a long
  press on the touchscreen (configurable duration, 100–5000 ms). The screen is read directly through evdev
  (`TouchHoldWatcher`), so a udev rule granting `uaccess` on the touchscreen is installed with the package
  (`70-plasma-keyboard-touchscreen.rules`). The global shortcut still shows the keyboard immediately.
- **Restarts itself after an update**: KWin keeps one keyboard process for the whole session, so an upgraded package
  would only take effect after logging out. The running instance watches its own binary and asks KWin to start the
  input method again (`kwinrc [Wayland] InputMethod` toggled), deferring the restart while the panel is visible but
  never for more than a couple of minutes — no manual restart after `pacman -Syu`.
- **Optional F1–F12 row** above the keyboard, sized and styled like the regular keys; the panel grows accordingly.
- **Clipboard row** above the keyboard (off by default): the recent entries of the desktop clipboard manager as three
  equally sized chips, aligned with the keyboard and centred while there are fewer than three. Tapping a chip inserts
  that text into the focused field (terminals included); long texts are shortened, and the row appears and disappears
  with the clipboard itself.
- **Single instance**: a second process exits right away, so a stale instance can never keep an old panel around.
- **Settings page** (`plasma-keyboard-custom` in System Settings), organised in tabs — *Layouts* (languages),
  *Opening* (long press, mouse focus, hiding the Plasma panel),   *Appearance* (height, font, F1–F12 row, clipboard row) and
  *Typing* (auto-capitalization, alternate characters, sound, vibration, navigation, a test field):
  - keyboard height as a percentage of the screen (20–80%),
  - whether the keyboard opens when a text field is focused with a mouse (otherwise it only opens on touch or via the shortcut),
  - open on long press with its threshold, the F1–F12 row, the keyboard font, hiding the Plasma panel while the
    keyboard is visible,
  - the page and its options are translated (the `kcm_plasmakeyboardcustom` translation domain is shipped with the
    package, Russian included) instead of falling back to English,
  - plus the upstream settings (locales, sound, vibration, navigation, diacritics, …).
- **Global shortcut** to show/hide the keyboard (default `Meta+Shift+K`, configurable in
  System Settings → Shortcuts → Plasma Keyboard (custom)).
- **Build/packaging**: a `PKGBUILD` for Arch-based systems that installs only custom-named files (no file conflicts with
  the official package), built by GitHub Actions on every release (the tag also gets a GitHub release with the package,
  the checksums and the pacman repository described above). The KCM's own translations are installed under our own domain
  (`/usr/share/locale/*/LC_MESSAGES/kcm_plasmakeyboardcustom.mo`); the application itself keeps using the
  "plasma-keyboard" translation domain provided by the official package, as installing it ourselves would conflict.

License and copyright remain those of the upstream project (see `LICENSES/` and the SPDX headers in each file).

## Install using the flatpak nightly repository

https://cdn.kde.org/flatpak/plasma-keyboard-nightly/org.kde.plasma.keyboard.flatpakref

See also: https://userbase.kde.org/Tutorials/Flatpak#Nightly_KDE_apps

## Development

Recommended methods for development are to either use
[KDE Linux](https://linux.kde.org/) (as your OS or in a VM), or to build the
Flatpak version of plasma-keyboard.

### KDE Linux (recommended)

Follow the
[instructions](https://linux.kde.org/docs/kde-dev/#build-kde-software-thats-shipped-on-the-base-image)
to set up a development environment for KDE software.

Build plasma-keyboard once to clone the source locally:

```bash
kde-builder plasma-keyboard
```

After making changes to the code the workflow is: rebuild, refresh sysext, and restart plasma-keyboard. This
script can be used to do that:

```bash
#!/usr/bin/env bash

set -e

# Rebuild plasma-keyboard and refresh the sysext
kde-builder --no-src plasma-keyboard && systemctl --user daemon-reload && sudo systemd-sysext refresh --always-refresh=yes && systemctl restart --user plasma-plasmashell.service

# Disable plasma-keyboard
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod ''

# Enable plasma-keyboard with the newly built changes
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod '/usr/share/applications/org.kde.plasma.keyboard.desktop'
```

### Flatpak

It is also possible to build plasma-keyboard as a Flatpak. This is the
recommended method for development on distributions other than KDE Linux, as we
don't want to install the development version of plasma-keyboard on a
traditional Linux distribution (which may break the system).

Clone the repository and make changes to the code, then build and install the
development version of the Flatpak with your changes by running the following
from the repository root (or save as a script and run it):

```bash
#!/usr/bin/env bash

# Build and install the flatpak
flatpak-builder --user --install --force-clean build-flatpak .flatpak-manifest.json

# Disable plasma-keyboard if it is already running
killall plasma-keyboard; kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod ''

# Enable plasma-keyboard with the newly built changes
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod '$HOME/.local/share/flatpak/exports/share/applications/org.kde.plasma.keyboard.desktop'

```

### Building from source manually

```sh
mkdir build && cd build
cmake ..
make && make install
```

### Troubleshooting

Join the [KDE Matrix chat](https://community.kde.org/Matrix) so we can help you
get started with development! We have a room specifically for input handling:
[#kde-input:kde.org](https://matrix.to/#/#kde-input:kde.org)

## Layouts

The keyboard layouts are located in the [src/layouts](/src/layouts) folder.

They are forked from Qt's [layouts](https://github.com/qt/qtvirtualkeyboard/tree/dev/src/layouts), with modifications that we want for Plasma. Please view the official [Qt documentation](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html#adding-new-keyboard-layouts) for a guide on how to create and modify keyboard layouts.

To use Qt's built-in keyboard layouts rather than the ones we supply in `plasma-keyboard`, set `PLASMA_KEYBOARD_USE_QT_LAYOUTS=1` when starting KWin (or the login session).

## Troubleshooting

KWin by default only shows the keyboard when a text field is interacted with by touch. Set `KWIN_IM_SHOW_ALWAYS=1` when starting KWin (or the login session) in order to force the keyboard to always pop up.

