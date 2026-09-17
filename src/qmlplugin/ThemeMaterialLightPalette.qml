// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#f2f0f4"
    textOnPrimaryColor: "#1b1b1d"
    secondaryColor: "#ffffff"
    textOnSecondaryColor: "#1b1b1d"

    keyboardBackgroundColor: "#f2f0f4"
    normalKeyBackgroundColor: "#ffffff"
    normalKeyPressedBackgroundColor: "#d9e2f8"
    highlightedKeyBackgroundColor: "#d9e2f8"
    latchedKeyBackgroundColor: "#a6c8ff"

    keyTextColor: "#1b1b1d"
    keySmallTextColor: "#7d7d7d"
    modeKeyAccentColor: "#1b1b1d"

    popupBackgroundColor: "#ffffff"
    popupTextColor: "#1b1b1d"
    popupTextSelectedColor: "#1b1b1d"
    popupHighlightBorderColor: "#a6c8ff"
    popupHighlightColor: Qt.rgba(0.651, 0.784, 1.0, 0.3)

    keyOutlineWidth: 0

    buttonRadius: 12
    popupRadius: 12

    // Material 3 "Default": white keys, a periwinkle container for the
    // special keys and a stronger blue for the primary action key.
    keyColors: ({
        modifier: {
            normal: "#d9e2f8",
            pressed: "#c3d3f5",
            highlighted: "#d9e2f8",
            latched: "#a6c8ff",
            active: "#a6c8ff",
            text: "#1b1b1d"
        },
        function: {
            normal: "#d9e2f8",
            pressed: "#c3d3f5",
            highlighted: "#d9e2f8",
            latched: "#a6c8ff",
            active: "#a6c8ff",
            text: "#1b1b1d"
        },
        accent: {
            normal: "#a6c8ff",
            pressed: "#8fbaff",
            highlighted: "#a6c8ff",
            latched: "#a6c8ff",
            active: "#a6c8ff",
            text: "#1b1b1d"
        }
    })
}
