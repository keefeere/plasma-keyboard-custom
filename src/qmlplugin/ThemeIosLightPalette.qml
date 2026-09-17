// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#d1d5db"
    textOnPrimaryColor: "#000000"
    secondaryColor: "#ffffff"
    textOnSecondaryColor: "#000000"

    keyboardBackgroundColor: "#d1d5db"
    normalKeyBackgroundColor: "#ffffff"
    normalKeyPressedBackgroundColor: "#b6bcc4"

    keyTextColor: "#000000"
    keySmallTextColor: "#000000"
    modeKeyAccentColor: "#000000"

    popupBackgroundColor: "#ffffff"
    popupTextColor: "#000000"
    popupTextSelectedColor: "#000000"

    keyLabelCase: "upper"
    keyShadowStrength: 0.4

    buttonRadius: 4
    popupRadius: 6

    keyColors: ({
        modifier: {
            // On iOS the modifier keys use the background grey.
            normal: keyboardBackgroundColor,
            pressed: normalKeyPressedBackgroundColor,
            text: keyTextColor
        }
    })
}
