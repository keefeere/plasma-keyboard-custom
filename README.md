<!--
  - SPDX-FileCopyrightText: None
  - SPDX-License-Identifier: CC0-1.0
-->

# Plasma Keyboard (custom fork)

> **This branch/repository is `plasma-keyboard-custom`**, a fork of
> [KDE plasma-keyboard](https://invent.kde.org/plasma/plasma-keyboard) with extra functionality for
> handheld / gamepad-driven use (MSI Claw, CachyOS + KDE Plasma 6 Wayland). It installs **next to**
> the official package and does not replace it — see
> [plasma-keyboard-custom (fork)](#plasma-keyboard-custom-fork) below.

The plasma-keyboard is a virtual keyboard based on [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html) designed to integrate in Plasma.

It wraps Qt Virtual Keyboard in a window, and uses the input-method-v1 Wayland protocol to communicate with the compositor to function as an input method.

## plasma-keyboard-custom (fork)

### Install the latest release

Download the newest `plasma-keyboard-custom-*-x86_64.pkg.tar.zst` from the
[Releases page](https://github.com/mops1k/plasma-keyboard-custom/releases/latest):

```sh
url=$(curl -fsSL https://api.github.com/repos/mops1k/plasma-keyboard-custom/releases/latest \
      | grep -o 'https://[^"]*\.pkg\.tar\.zst' | head -n1)
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

**To update**, run exactly the same commands — the package version (and `pkgrel`) grows with every
release, so `pacman -U` upgrades the installed package in place. Then restart the keyboard so the new
binary is picked up (or log out and back in):

```sh
pkill -f 'plasma-keyboard-custom'
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod ''
kwriteconfig6 --notify --file kwinrc --group Wayland --key InputMethod '/usr/share/applications/org.kde.plasma.keyboard.custom.desktop'
```

Then pick **plasma-keyboard-custom** in **System Settings → Virtual Keyboard**; its own settings are under
**System Settings → Plasma Keyboard (custom)**. It installs next to the official `plasma-keyboard` package.

To build the package yourself: `bash packaging/build.sh`.

### About

This is a **fork of [KDE plasma-keyboard](https://invent.kde.org/plasma/plasma-keyboard)** (based on the 6.7.90 sources) with
extra functionality for handheld / gamepad-driven use, primarily tested on an MSI Claw running CachyOS + KDE Plasma 6 Wayland.

It installs next to the official package and does not replace it: everything is renamed
(`plasma-keyboard-custom` binary, `org.kde.plasma.keyboard.custom*` QML modules, `kcm_plasmakeyboardcustom`, layouts in
`share/plasma/keyboard-custom`, style `PlasmaBreeze`), so both the stock and the custom keyboard show up under
**System Settings → Virtual Keyboard** and can be selected there.

### What is different from upstream

- **Gamepad support** via InputPlumber's system D-Bus target (`org.shadowblip.Input.DBusDevice`):
  - D-pad / left stick navigate, **A** selects, **B** closes, **X** backspace, **Y** space, **LT** shift, **RT** enter,
    **LB** symbols, **RB** switches the layout.
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
- **Open on long press**: instead of popping up as soon as a text field is focused, the keyboard can wait for a long
  press on the touchscreen (configurable duration, 100–5000 ms). The screen is read directly through evdev
  (`TouchHoldWatcher`), so a udev rule granting `uaccess` on the touchscreen is installed with the package
  (`70-plasma-keyboard-touchscreen.rules`). The global shortcut still shows the keyboard immediately.
- **Optional F1–F12 row** above the keyboard, sized and styled like the regular keys; the panel grows accordingly.
- **Single instance**: a second process exits right away, so a stale instance can never keep an old panel around.
- **Settings page** (`plasma-keyboard-custom` in System Settings):
  - keyboard height as a percentage of the screen (20–80%),
  - whether the keyboard opens when a text field is focused with a mouse (otherwise it only opens on touch or via the shortcut),
  - open on long press with its threshold, the F1–F12 row, the keyboard font, hiding the Plasma panel while the
    keyboard is visible,
  - plus the upstream settings (locales, sound, vibration, navigation, diacritics, …).
- **Global shortcut** to show/hide the keyboard (default `Meta+Shift+K`, configurable in
  System Settings → Shortcuts → Plasma Keyboard (custom)).
- **Build/packaging**: a `PKGBUILD` for Arch-based systems that installs only custom-named files (no file conflicts with
  the official package). Translations (`.mo`) are intentionally not installed to avoid conflicts; the "plasma-keyboard"
  translation domain from the official package is reused when present.

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

