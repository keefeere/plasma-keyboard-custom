// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#fcfcfc"
    textOnPrimaryColor: "#232629"
    secondaryColor: "#ffffff"
    textOnSecondaryColor: "#232629"

    keyboardBackgroundColor: "#eff0f1"
    normalKeyBackgroundColor: "#ffffff"
    normalKeyPressedBackgroundColor: "#bdc3c7"
    highlightedKeyBackgroundColor: "#e3e5e7"
    latchedKeyBackgroundColor: "#b3d4f5"

    keyTextColor: "#232629"
    keySmallTextColor: "#232629"

    popupBackgroundColor: "#ffffff"
    popupTextColor: "#232629"
    popupTextSelectedColor: "#232629"
    popupHighlightBorderColor: "#3daee9"
    popupHighlightColor: Qt.rgba(0.239, 0.682, 0.914, 0.3)

    selectionListTextColor: "#232629"
    selectionListSeparatorColor: "#bdc3c7"
    selectionListBackgroundColor: "#eff0f1"

    navigationHighlightBorderColor: "#3daee9"
    navigationHighlightColor: Qt.rgba(0.239, 0.682, 0.914, 0.3)

    keyColors: ({
        modifier: {
            normal: Qt.darker(normalKeyBackgroundColor, 1.12),
            pressed: Qt.darker(normalKeyPressedBackgroundColor, 1.1),
            text: keyTextColor
        }
    })
}
