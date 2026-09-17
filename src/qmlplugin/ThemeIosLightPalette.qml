// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#d1d2d5"
    textOnPrimaryColor: "#000000"
    secondaryColor: "#ffffff"
    textOnSecondaryColor: "#000000"

    keyboardBackgroundColor: "#d1d2d5"
    normalKeyBackgroundColor: "#ffffff"
    normalKeyPressedBackgroundColor: "#d1d2d5"
    highlightedKeyBackgroundColor: "#d1d2d5"
    latchedKeyBackgroundColor: "#ffffff"

    keyTextColor: "#000000"
    keySmallTextColor: "#7d7e80"
    modeKeyAccentColor: "#000000"

    popupBackgroundColor: "#ffffff"
    popupTextColor: "#000000"
    popupTextSelectedColor: "#000000"
    popupHighlightBorderColor: "#0a84ff"
    popupHighlightColor: Qt.rgba(0.039, 0.518, 1.0, 0.3)

    keyShadowStrength: 0.4

    buttonRadius: 6
    popupRadius: 10

    // The iPad keys: white letter keys, grey special keys, grey-blue press.
    keyColors: ({
        modifier: {
            normal: "#989aa3",
            pressed: "#7d7e80",
            highlighted: "#989aa3",
            latched: "#ffffff",
            active: "#ffffff",
            text: "#000000"
        },
        function: {
            normal: "#989aa3",
            pressed: "#7d7e80",
            highlighted: "#989aa3",
            text: "#000000"
        },
        accent: {
            normal: "#989aa3",
            pressed: "#7d7e80",
            highlighted: "#989aa3",
            text: "#000000"
        }
    })
}
