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

[Русская версия](README.ru.md)

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

> [!IMPORTANT]
> The prebuilt package needs **SteamOS 3.8 or newer** (it is built against Qt 6.9, which is what
> SteamOS 3.8 ships). SteamOS 3.7 and older carry Qt 6.7/6.8 and cannot load the binaries — there,
> build the package from source.

On SteamOS that fails on `libstdc++`: the package is built against the current Arch, where the C++
runtime recently became a package of its own, while SteamOS carries a frozen snapshot in which
`libstdc++.so.6` belongs to `gcc-libs` and no `libstdc++` package exists to download. The library
itself is already there, so tell pacman to treat the dependency as satisfied:

```sh
sudo pacman -U --assume-installed libstdc++ /tmp/plasma-keyboard-custom.pkg.tar.zst
```

The [install script](install.sh) adds that flag by itself when the repositories have no `libstdc++`,
and it works the same way for an install from the repository:
`sudo pacman -S --assume-installed libstdc++ plasma-keyboard-custom`.

The release is built against Qt 6.9, the version SteamOS 3.8 ships, so the binaries carry the
`Qt_6.9` symbol version and load there as well as on current Arch/CachyOS (Qt 6.11). The app uses
Qt's private Gui and WaylandClient APIs, whose ABI changes between Qt minor releases, so the build
is pinned to the Arch snapshot matching SteamOS 3.8 (Qt 6.9.1-5) instead of the rolling Qt. SteamOS
3.7 and older ship Qt 6.7/6.8 and are **not supported** by the prebuilt package — on those systems
build it locally from source.

On SteamOS the system is mounted read-only, so the script runs
`sudo steamos-readonly disable` before installing and `sudo steamos-readonly enable` afterwards — also
when the installation fails. Installing by hand there needs the same two commands around `pacman`.

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

On SteamOS this needs the same handling as above, since its repositories have no `libstdc++`:

```sh
sudo pacman -S --assume-installed libstdc++ plasma-keyboard-custom
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
[`.github/workflows/deploy-repo.yml`](.github/workflows/deploy-repo.yml), which the release workflow
calls as a reusable one, so it runs — and has to succeed — inside the same run that publishes the
release. The workflow can also be re-run by hand from the Actions tab: with a release tag it
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

- **Gamepad navigation** reaches the rows above the keyboard too, not only the key grid: up from the top row goes
  into the F1-F12 row (landing under the selected key) and from there into the clipboard row, down continues back,
  left/right move along a row, A activates the selected item and Select jumps into the rows. The focused item is
  highlighted the same way Qt Virtual Keyboard highlights its keys, the clipboard row scrolls the selected entry
  into view, and disabled or empty rows simply drop out of the chain. Full gamepad support is provided via
  InputPlumber's system D-Bus target (`org.shadowblip.Input.DBusDevice`):
  - D-pad / left stick navigate, **A** selects, **B** closes, **X** backspace (holding it keeps deleting, like a key on a
    hardware keyboard), **Y** space, **LT** shift, **RT** enter, **LB** symbols, **RB** switches the layout.
  - **Holding A opens the alternate characters of the highlighted key** — the characters come from the layout (its
    `alternativeKeys`, the very ones a long press on that same on-screen key shows: `e`→`é`, `1`→`!`), but without
    typing the base character. The list appears **over that key** and is drawn with the keyboard style's own
    alternate-keys components, so it follows the theme the user chose. The D-pad (or stick) walks the options — the
    list itself stays put and only the highlight moves; **A** takes the selected character, **B** closes the list
    without picking anything (a second press closes the keyboard). A key with a single alternate inserts it right
    away, without a list. Tapping A still just types the key: the list only opens after the hold delay (400 ms by
    default), which can be changed or disabled in the KCM.
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
- **Themes**: seven built-in themes (system, light/dark, and iOS and Material in their light and dark variants) plus
  user themes imported as plain JSON (palette, geometry, background, key style and per-category key colours), selectable
  live from the settings page. A theme file is data and never code — see [Themes](#themes).
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
- **Clipboard and suggestion row** above the keyboard (the clipboard is off by default): the recent entries of the desktop
  clipboard manager, asked for over D-Bus (`org.kde.klipper`, i.e. the clipboard widget of the Plasma panel), shown as three
  equally sized chips that are aligned with the keyboard and centred while there are fewer than three. Tapping a chip inserts
  that text into the focused field (terminals included), the key at the right edge of the row forgets the whole history, and
  long or multi-line entries are shortened to a single line. While a word is being typed the same row offers the word
  suggestions (below), and the key at its right edge switches the row between the two; the row appears and disappears
  together with them.
- **Predictive text (word suggestions)**: while a word is being typed, the words that continue it are offered above the
  keyboard, the most frequent first (three at a time by default, 1–5 in the settings). The words come from frequency
  dictionaries of Russian and English (about 1.18 M and 1.03 M word forms, FrequencyWords data, MIT licence), and the
  prediction engine is our own — the hunspell plugin of Qt Virtual Keyboard stays deliberately switched off. Suggestions
  start with the first letter (the threshold is configurable, 1–4), the chips are as wide as their text and sit on the
  left, and the part that would complete the word is underlined. Taking a chip (or **A** on a gamepad) replaces the word
  being typed as a whole and **adds a space**, so that the next word can be typed right away. It works with touch, mouse
  and gamepad (D-pad along the row, **A** to take), and is configured on the *Typing* tab.
- **Single instance**: a second process exits right away, so a stale instance can never keep an old panel around.
- **Settings page** (`plasma-keyboard-custom` in System Settings), organised in tabs — *Layouts* (languages),
  *Opening* (long press, mouse focus, hiding the Plasma panel),   *Appearance* (height, font, F1–F12 row, clipboard row) and
  *Typing* (auto-capitalization, word suggestions, alternate characters, sound, vibration, navigation, a test field):
  - keyboard height as a percentage of the screen (20–80%),
  - whether the keyboard opens when a text field is focused with a mouse (otherwise it only opens on touch or via the shortcut),
  - open on long press with its threshold, the F1–F12 row, the keyboard font, hiding the Plasma panel while the
    keyboard is visible,
  - word suggestions: on or off, how many are offered at once (1–5) and how many letters it takes for them to appear (1–4),
  - which of the enabled layouts opens by default — a star next to it in the layout list; without a choice the layout
    the system asks for decides, falling back to the first enabled one,
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

## Building from source manually

```sh
mkdir build && cd build
cmake ..
make && make install
```

## Layouts

The keyboard layouts are located in the [src/layouts](/src/layouts) folder.

They are forked from Qt's [layouts](https://github.com/qt/qtvirtualkeyboard/tree/dev/src/layouts), with modifications that we want for Plasma. Please view the official [Qt documentation](https://doc.qt.io/qt-6/qtvirtualkeyboard-overview.html#adding-new-keyboard-layouts) for a guide on how to create and modify keyboard layouts.

To use Qt's built-in keyboard layouts rather than the ones we supply in `plasma-keyboard`, set `PLASMA_KEYBOARD_USE_QT_LAYOUTS=1` when starting KWin (or the login session).

## Themes

Plasma Keyboard (custom) paints the keyboard from a palette. Seven themes ship with it, and you can add your own as JSON files. A theme carries only colours and a few geometry/style values, so a theme file is **data and never code** — importing a file written by someone else cannot execute anything.

Pick a theme in **System Settings → Plasma Keyboard (custom) → Appearance → Theme**. It is applied to a running keyboard immediately, without a restart.

### Built-in themes

| Id | Name | What it is |
| --- | --- | --- |
| `system` | System | The default. Overrides nothing and follows the current Plasma colour scheme (`Kirigami.Theme`). |
| `light` | Light | The system palette with lighter keys. |
| `dark` | Dark | The system palette with darker keys. |
| `ios-light` | iOS (light) | Apple's light keyboard: white keys on a grey background, flat, upper-case labels, 6 px corners. |
| `ios-dark` | iOS (dark) | The dark iOS variant. |
| `material-light` | Material (light) | Material 3 "Default": white keys on a light surface, blue-grey accents, 12 px corners, no outline. |
| `material-dark` | Material (dark) | The dark Material 3 variant. |

### User themes

User themes live in:

```
~/.local/share/plasma-keyboard/themes/*.json
```

(or `$XDG_DATA_HOME/plasma-keyboard/themes` when `XDG_DATA_HOME` is set). The directory is created on the first import and is not part of the package. The file name is the theme's id: the display name is turned into a lower-case slug (`Midnight Ocean` → `midnight-ocean.json`), and that id is what the settings page stores.

A theme is a JSON object. Only the keys below are accepted — an unknown key or a wrong type is rejected with an error rather than silently ignored, so a typo cannot do nothing. Every colour must be a valid colour string (a CSS name such as `red`, or `#rgb`, `#rrggbb` or `#aarrggbb`). Missing values are taken from the `base` theme.

| Key | Type | Meaning |
| --- | --- | --- |
| `name` | string | The display name. Optional; without it the file name is used. |
| `base` | string | The built-in theme the unspecified values come from. Optional, defaults to `system`; when present it must be one of the seven ids above. |
| `palette` | object | Palette properties to override (see the list below). |
| `geometry` | object | `keyBackgroundMargin`, `keyContentMargin`, `keyIconScale`, `buttonRadius`, `popupRadius` — all numbers. The keyboard font is **not** part of a theme; it is the separate *Keyboard font* setting. |
| `background` | object | `type` (`"color"` or `"gradient"`), `start` and `end` (colours), and `angle`. The angle accepts only `0`, `90`, `180` or `270` (it is only meaningful for a gradient): `0`/`180` draw vertically and `90`/`270` horizontally, because Qt's `Rectangle.gradient` supports only those two orientations. |
| `keyStyle` | object | `outlineWidth` (number), `outlineColor` (colour), `shadowStrength` (number) and `labelCase` (`"normal"` or `"upper"`). |
| `keyColors` | object | Per-category key colours and outlines (see below). |

`palette` accepts exactly these colour properties:

- base colours: `primaryColor`, `primaryLightColor`, `primaryDarkColor`, `textOnPrimaryColor`, `secondaryColor`, `secondaryLightColor`, `secondaryDarkColor`, `textOnSecondaryColor`
- keyboard and keys: `keyboardBackgroundColor`, `normalKeyBackgroundColor`, `normalKeyPressedBackgroundColor`, `highlightedKeyBackgroundColor`, `latchedKeyBackgroundColor`, `capsLockKeyAccentColor`, `modeKeyAccentColor`, `keyTextColor`, `keySmallTextColor`
- popups: `popupBackgroundColor`, `popupBorderColor`, `popupTextColor`, `popupTextSelectedColor`, `popupHighlightBorderColor`, `popupHighlightColor`
- selection list: `selectionListTextColor`, `selectionListSeparatorColor`, `selectionListBackgroundColor`
- navigation highlight: `navigationHighlightColor`, `navigationHighlightBorderColor`

`keyColors` maps a **category** to the colours for that category's **states**. The six categories are:

- `suggestions` — the clipboard chips and their clear button,
- `modifier` — `Shift`, `Ctrl`, `Alt`, `AltGr`, `Meta`, `CapsLock`,
- `function` — keys marked as function keys,
- `accent` — `Enter`,
- `digit` — keys with a single non-letter symbol (digits, `=`/`-`, shifted symbols, punctuation),
- `normal` — everything else, including the space bar.

Inside a category the recognised states are `normal`, `pressed`, `highlighted`, `latched`, `active` and `text` (all colours; `text` is the label colour). A category can additionally set `outlineWidth` (number), `outlineColor` (colour), `shadow` (number) or an `outline` object `{ "width": <number>, "color": <colour> }`. A state that is not set falls back in this order: the category's colour for that state → the category's `normal` → the `normal` category's colour for the state → the `normal` category's `normal` → the global palette property above.

A complete example — valid as written and safe to copy into a file and import:

```json
{
    "name": "Midnight Ocean",
    "base": "dark",
    "palette": {
        "primaryColor": "#16233f",
        "primaryLightColor": "#1b2a4a",
        "primaryDarkColor": "#0f1b33",
        "textOnPrimaryColor": "#e8f0ff",
        "secondaryColor": "#101a33",
        "textOnSecondaryColor": "#e8f0ff",
        "keyboardBackgroundColor": "#0b1020",
        "normalKeyBackgroundColor": "#1b2a4a",
        "normalKeyPressedBackgroundColor": "#0f1b33",
        "highlightedKeyBackgroundColor": "#26427a",
        "latchedKeyBackgroundColor": "#3a5fa8",
        "capsLockKeyAccentColor": "#4c8dff",
        "modeKeyAccentColor": "#4c8dff",
        "keyTextColor": "#e8f0ff",
        "keySmallTextColor": "#9fb8e6",
        "popupBackgroundColor": "#101a33",
        "popupTextColor": "#e8f0ff",
        "popupHighlightBorderColor": "#4c8dff",
        "popupHighlightColor": "#4c8dff4d",
        "selectionListTextColor": "#e8f0ff",
        "selectionListBackgroundColor": "#0b1020",
        "navigationHighlightColor": "#4c8dff4d",
        "navigationHighlightBorderColor": "#4c8dff"
    },
    "geometry": {
        "keyBackgroundMargin": 8,
        "keyContentMargin": 40,
        "keyIconScale": 0.8,
        "buttonRadius": 10,
        "popupRadius": 12
    },
    "background": {
        "type": "gradient",
        "start": "#101a33",
        "end": "#05070f",
        "angle": 180
    },
    "keyStyle": {
        "outlineWidth": 1,
        "outlineColor": "#2a3f6b",
        "shadowStrength": 0.5,
        "labelCase": "normal"
    },
    "keyColors": {
        "normal": {
            "normal": "#1b2a4a",
            "pressed": "#0f1b33",
            "highlighted": "#26427a",
            "text": "#e8f0ff"
        },
        "modifier": {
            "normal": "#24365c",
            "pressed": "#16233f",
            "highlighted": "#2d4877",
            "latched": "#3a5fa8",
            "active": "#4c8dff",
            "text": "#ffffff"
        },
        "function": {
            "normal": "#152036",
            "pressed": "#0d1526",
            "text": "#9fb8e6",
            "outline": {
                "width": 1,
                "color": "#2a3f6b"
            },
            "shadow": 0.5
        },
        "accent": {
            "normal": "#4c8dff",
            "pressed": "#3a6ecc",
            "text": "#00121f"
        },
        "digit": {
            "normal": "#1b2a4a",
            "pressed": "#0f1b33",
            "text": "#cfe0ff"
        },
        "suggestions": {
            "normal": "#22345a",
            "pressed": "#16233f",
            "text": "#e8f0ff"
        }
    }
}
```

### Import, export and removal

The *Theme files* row in the same **Appearance** tab manages user themes; errors are shown inline under it.

- **Import theme…** opens a file dialog for a `*.json` file and validates it before copying it into the theme directory. A broken file, an unknown key or category, a wrong type, an unknown `base`, an empty `name` or a duplicate name are all reported instead of being installed.
- **Export theme…** writes the selected theme to a file. For the theme that is currently applied it writes a full snapshot (the base plus every effective palette, geometry, background, key style and key colour value), so re-importing it reproduces the same look on its own. A user theme that is not currently applied is written as its stored `base` plus its overrides; a built-in that is not currently applied is written as its `base` alone.
- **Remove theme** is enabled only for user themes (the name of a built-in is greyed out). It asks for confirmation; if the removed theme was the selected one, the selection falls back to `system`. Built-in themes cannot be removed.

A user theme can also be deleted by hand from `~/.local/share/plasma-keyboard/themes/`; a theme that disappears while it is selected also falls back to `system`.

## Troubleshooting

KWin by default only shows the keyboard when a text field is interacted with by touch. Set `KWIN_IM_SHOW_ALWAYS=1` when starting KWin (or the login session) in order to force the keyboard to always pop up.

## Credits

This is a fork of KDE's [plasma-keyboard](https://invent.kde.org/plasma/plasma-keyboard), which is
built on the [Qt Virtual Keyboard](https://doc.qt.io/qt-6/qtvirtualkeyboard-index.html). Many thanks
to the plasma-keyboard authors and to the KDE community for the original application, the Breeze
style, the layouts and the translations, and to The Qt Company for the virtual keyboard framework —
this fork would not exist without their work.

The fork is maintained as
[mops1k/plasma-keyboard-custom](https://github.com/mops1k/plasma-keyboard-custom); bug reports and
patches for the fork's own features (gamepad support, the clipboard row, the function key row, the
theme system and the rest) are welcome there.
