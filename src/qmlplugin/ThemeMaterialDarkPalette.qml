// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#121212"
    textOnPrimaryColor: "#ffffff"
    secondaryColor: "#1e1e1e"
    textOnSecondaryColor: "#ffffff"

    keyboardBackgroundColor: "#121212"
    normalKeyBackgroundColor: "#1e1e1e"
    normalKeyPressedBackgroundColor: "#333333"

    keyTextColor: "#ffffff"
    keySmallTextColor: "#ffffff"
    modeKeyAccentColor: "#ffffff"

    popupBackgroundColor: "#1e1e1e"
    popupTextColor: "#ffffff"
    popupTextSelectedColor: "#ffffff"

    keyOutlineWidth: 1
    keyOutlineColor: "#373737"

    buttonRadius: 4
    popupRadius: 4

    keyColors: ({
        modifier: {
            normal: "#2a2a2a",
            pressed: "#3a3a3a",
            text: keyTextColor
        }
    })

    backgroundType: "gradient"
    backgroundStart: "#1f1f1f"
    backgroundEnd: "#0d0d0d"
    backgroundAngle: 0
}
