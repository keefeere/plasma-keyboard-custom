// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#1c1c1e"
    textOnPrimaryColor: "#ffffff"
    secondaryColor: "#3a3a3c"
    textOnSecondaryColor: "#ffffff"

    keyboardBackgroundColor: "#1c1c1e"
    normalKeyBackgroundColor: "#3a3a3c"
    normalKeyPressedBackgroundColor: "#2c2c2e"

    keyTextColor: "#ffffff"
    keySmallTextColor: "#ffffff"
    modeKeyAccentColor: "#ffffff"

    popupBackgroundColor: "#3a3a3c"
    popupTextColor: "#ffffff"
    popupTextSelectedColor: "#ffffff"

    keyLabelCase: "upper"

    buttonRadius: 4
    popupRadius: 6

    keyColors: ({
        modifier: {
            normal: "#2c2c2e",
            pressed: "#232325",
            text: keyTextColor
        }
    })
}
