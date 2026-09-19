// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#11111b"
    primaryLightColor: "#181825"
    primaryDarkColor: "#313244"
    textOnPrimaryColor: "#cdd6f4"
    secondaryColor: "#1e1e2e"
    secondaryLightColor: "#313244"
    secondaryDarkColor: "#181825"
    textOnSecondaryColor: "#cdd6f4"

    keyboardBackgroundColor: "#11111b"
    normalKeyBackgroundColor: "#181825"
    normalKeyPressedBackgroundColor: "#313244"
    highlightedKeyBackgroundColor: "#313244"
    latchedKeyBackgroundColor: "#89b4fa"
    capsLockKeyAccentColor: "#89b4fa"
    modeKeyAccentColor: "#cdd6f4"
    keyTextColor: "#cdd6f4"
    keySmallTextColor: "#a6adc8"

    popupBackgroundColor: "#1e1e2e"
    popupBorderColor: "#313244"
    popupTextColor: "#cdd6f4"
    popupTextSelectedColor: "#11111b"
    popupHighlightBorderColor: "#89b4fa"
    popupHighlightColor: Qt.rgba(0.537, 0.706, 0.980, 0.3)
    selectionListTextColor: "#cdd6f4"
    selectionListSeparatorColor: "#313244"
    selectionListBackgroundColor: "#11111b"
    navigationHighlightColor: Qt.rgba(0.537, 0.706, 0.980, 0.3)
    navigationHighlightBorderColor: "#89b4fa"

    keyBackgroundMargin: 4
    buttonRadius: 14
    popupRadius: 14

    keyOutlineWidth: 0
    keyShadowStrength: 0
    keyLabelCase: "normal"

    // The dark Catppuccin Mocha palette (https://catppuccin.com/palette): a
    // near-black panel, mantle letter keys, surface0 special keys and a blue
    // primary action key. The keys are flat (no outline, no shadow) and the
    // labels stay in lower case, and the background margin is small so that the
    // keys are as large as the panel allows.
    keyColors: ({
        normal: {
            normal: "#181825",
            pressed: "#313244",
            highlighted: "#313244",
            latched: "#89b4fa",
            active: "#89b4fa",
            text: "#cdd6f4"
        },
        digit: {
            normal: "#181825",
            pressed: "#313244",
            highlighted: "#313244",
            text: "#cdd6f4"
        },
        modifier: {
            normal: "#313244",
            pressed: "#45475a",
            highlighted: "#313244",
            latched: "#89b4fa",
            active: "#89b4fa",
            text: "#cdd6f4"
        },
        function: {
            normal: "#313244",
            pressed: "#45475a",
            highlighted: "#313244",
            text: "#cdd6f4"
        },
        accent: {
            normal: "#89b4fa",
            pressed: "#74a0e0",
            highlighted: "#89b4fa",
            latched: "#89b4fa",
            active: "#89b4fa",
            text: "#ffffff"
        },
        suggestions: {
            normal: "#1e1e2e",
            pressed: "#313244",
            highlighted: "#313244",
            text: "#89b4fa"
        }
    })
}
