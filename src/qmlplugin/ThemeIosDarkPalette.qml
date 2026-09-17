// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#323233"
    textOnPrimaryColor: "#ffffff"
    secondaryColor: "#565656"
    textOnSecondaryColor: "#ffffff"

    keyboardBackgroundColor: "#323233"
    normalKeyBackgroundColor: "#828282"
    normalKeyPressedBackgroundColor: "#6f6f70"
    highlightedKeyBackgroundColor: "#6f6f70"
    latchedKeyBackgroundColor: "#a0a0a0"

    keyTextColor: "#ffffff"
    keySmallTextColor: "#c0c0c0"
    modeKeyAccentColor: "#ffffff"

    popupBackgroundColor: "#565656"
    popupTextColor: "#ffffff"
    popupTextSelectedColor: "#ffffff"
    popupHighlightBorderColor: "#0a84ff"
    popupHighlightColor: Qt.rgba(0.039, 0.518, 1.0, 0.3)

    keyShadowStrength: 0.25

    buttonRadius: 6
    popupRadius: 10

    // The iPad keys: light grey letter keys on a dark panel, darker special
    // keys, and a dimmer press.
    keyColors: ({
        modifier: {
            normal: "#565656",
            pressed: "#454546",
            highlighted: "#565656",
            latched: "#a0a0a0",
            active: "#a0a0a0",
            text: "#ffffff"
        },
        function: {
            normal: "#565656",
            pressed: "#454546",
            highlighted: "#565656",
            text: "#ffffff"
        },
        accent: {
            normal: "#565656",
            pressed: "#454546",
            highlighted: "#565656",
            text: "#ffffff"
        }
    })
}
