// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#e8eaed"
    primaryLightColor: "#ffffff"
    primaryDarkColor: "#ccced5"
    textOnPrimaryColor: "#3c4043"
    secondaryColor: "#ffffff"
    secondaryLightColor: "#f1f3f4"
    secondaryDarkColor: "#e8eaed"
    textOnSecondaryColor: "#3c4043"

    keyboardBackgroundColor: "#e8eaed"
    normalKeyBackgroundColor: "#ffffff"
    normalKeyPressedBackgroundColor: "#e8eaed"
    highlightedKeyBackgroundColor: "#f1f3f4"
    latchedKeyBackgroundColor: "#1a73e8"
    capsLockKeyAccentColor: "#1a73e8"
    modeKeyAccentColor: "#5f6368"
    keyTextColor: "#3c4043"
    keySmallTextColor: "#5f6368"

    popupBackgroundColor: "#ffffff"
    popupBorderColor: "#dadce0"
    popupTextColor: "#202124"
    popupTextSelectedColor: "#ffffff"
    popupHighlightBorderColor: "#1a73e8"
    popupHighlightColor: Qt.rgba(0.102, 0.451, 0.910, 0.3)
    selectionListTextColor: "#3c4043"
    selectionListSeparatorColor: "#dadce0"
    selectionListBackgroundColor: "#ffffff"
    navigationHighlightColor: Qt.rgba(0.102, 0.451, 0.910, 0.3)
    navigationHighlightBorderColor: "#1a73e8"

    keyBackgroundMargin: 4
    buttonRadius: 18
    popupRadius: 14

    keyOutlineWidth: 0
    keyShadowStrength: 0
    keyLabelCase: "normal"

    // Gboard in its light colours: white keys on a light grey panel, light grey
    // special keys and the Google Blue action key. The keys are flat (no outline,
    // no shadow), round and close together, and the labels stay in lower case.
    keyColors: ({
        normal: {
            normal: "#ffffff",
            pressed: "#e8eaed",
            highlighted: "#f1f3f4",
            latched: "#1a73e8",
            active: "#1a73e8",
            text: "#3c4043"
        },
        digit: {
            normal: "#ffffff",
            pressed: "#e8eaed",
            highlighted: "#f1f3f4",
            text: "#3c4043"
        },
        modifier: {
            normal: "#ccced5",
            pressed: "#bfc1c8",
            highlighted: "#ccced5",
            latched: "#1a73e8",
            active: "#1a73e8",
            text: "#3c4043"
        },
        function: {
            normal: "#ccced5",
            pressed: "#bfc1c8",
            highlighted: "#ccced5",
            text: "#3c4043"
        },
        accent: {
            normal: "#1a73e8",
            pressed: "#1765cc",
            highlighted: "#1a73e8",
            latched: "#1a73e8",
            active: "#1a73e8",
            text: "#ffffff"
        },
        suggestions: {
            normal: "#ffffff",
            pressed: "#e8eaed",
            highlighted: "#f1f3f4",
            text: "#3c4043"
        }
    })
}
