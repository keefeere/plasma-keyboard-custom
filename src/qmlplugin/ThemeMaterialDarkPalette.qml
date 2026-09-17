// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#1b1d1f"
    textOnPrimaryColor: "#ffffff"
    secondaryColor: "#23262a"
    textOnSecondaryColor: "#ffffff"

    keyboardBackgroundColor: "#1b1d1f"
    normalKeyBackgroundColor: "#23262a"
    normalKeyPressedBackgroundColor: "#3a3e44"
    highlightedKeyBackgroundColor: "#3a3e44"
    latchedKeyBackgroundColor: "#4f6375"

    keyTextColor: "#ffffff"
    keySmallTextColor: "#adaeaf"
    modeKeyAccentColor: "#ffffff"

    popupBackgroundColor: "#23262a"
    popupTextColor: "#ffffff"
    popupTextSelectedColor: "#ffffff"
    popupHighlightBorderColor: "#588ab9"
    popupHighlightColor: Qt.rgba(0.345, 0.541, 0.725, 0.3)

    keyOutlineWidth: 0

    buttonRadius: 12
    popupRadius: 12

    // Material 3 "Default" dark: near-black keys, a steel-blue container for
    // the special keys and a brighter blue for the primary action key.
    keyColors: ({
        modifier: {
            normal: "#4f6375",
            pressed: "#425365",
            highlighted: "#4f6375",
            latched: "#588ab9",
            active: "#588ab9",
            text: "#ffffff"
        },
        function: {
            normal: "#4f6375",
            pressed: "#425365",
            highlighted: "#4f6375",
            latched: "#588ab9",
            active: "#588ab9",
            text: "#ffffff"
        },
        accent: {
            normal: "#588ab9",
            pressed: "#497798",
            highlighted: "#588ab9",
            latched: "#588ab9",
            active: "#588ab9",
            text: "#ffffff"
        }
    })
}
