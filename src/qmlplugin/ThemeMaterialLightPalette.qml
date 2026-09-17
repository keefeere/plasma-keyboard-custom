// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#eceff1"
    textOnPrimaryColor: "#212121"
    secondaryColor: "#ffffff"
    textOnSecondaryColor: "#212121"

    keyboardBackgroundColor: "#eceff1"
    normalKeyBackgroundColor: "#ffffff"
    normalKeyPressedBackgroundColor: "#e0e0e0"

    keyTextColor: "#212121"
    keySmallTextColor: "#212121"
    modeKeyAccentColor: "#212121"

    popupBackgroundColor: "#ffffff"
    popupTextColor: "#212121"
    popupTextSelectedColor: "#212121"

    keyOutlineWidth: 1
    keyOutlineColor: "#b0bec5"

    buttonRadius: 4
    popupRadius: 4

    keyColors: ({
        modifier: {
            normal: "#e0e0e0",
            pressed: "#cfd8dc",
            text: keyTextColor
        }
    })

    backgroundType: "gradient"
    backgroundStart: "#f5f7f8"
    backgroundEnd: "#dfe3e6"
    backgroundAngle: 0
}
