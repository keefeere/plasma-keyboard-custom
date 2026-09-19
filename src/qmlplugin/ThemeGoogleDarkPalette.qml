// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    primaryColor: "#263238"
    primaryLightColor: "#404a50"
    primaryDarkColor: "#313c42"
    textOnPrimaryColor: "#e8eaed"
    secondaryColor: "#313c42"
    secondaryLightColor: "#404a50"
    secondaryDarkColor: "#263238"
    textOnSecondaryColor: "#e8eaed"

    keyboardBackgroundColor: "#263238"
    normalKeyBackgroundColor: "#404a50"
    normalKeyPressedBackgroundColor: "#313c42"
    highlightedKeyBackgroundColor: "#4a555c"
    latchedKeyBackgroundColor: "#6eaca8"
    capsLockKeyAccentColor: "#6eaca8"
    modeKeyAccentColor: "#d9dbdc"
    keyTextColor: "#e8eaed"
    keySmallTextColor: "#a5a9ac"

    popupBackgroundColor: "#313c42"
    popupBorderColor: "#404a50"
    popupTextColor: "#e8eaed"
    popupTextSelectedColor: "#263238"
    popupHighlightBorderColor: "#6eaca8"
    popupHighlightColor: Qt.rgba(0.431, 0.675, 0.659, 0.3)
    selectionListTextColor: "#e8eaed"
    selectionListSeparatorColor: "#404a50"
    selectionListBackgroundColor: "#263238"
    navigationHighlightColor: Qt.rgba(0.431, 0.675, 0.659, 0.3)
    navigationHighlightBorderColor: "#6eaca8"

    keyBackgroundMargin: 4
    buttonRadius: 18
    popupRadius: 14

    keyOutlineWidth: 0
    keyShadowStrength: 0
    keyLabelCase: "normal"

    // Gboard in its dark colours: grey-blue keys on a darker panel, darker
    // special keys and the teal action key of the dark reference. Flat, round
    // keys with narrow gaps and lower-case labels.
    keyColors: ({
        normal: {
            normal: "#404a50",
            pressed: "#313c42",
            highlighted: "#4a555c",
            latched: "#6eaca8",
            active: "#6eaca8",
            text: "#e8eaed"
        },
        digit: {
            normal: "#404a50",
            pressed: "#313c42",
            highlighted: "#4a555c",
            text: "#e8eaed"
        },
        modifier: {
            normal: "#313c42",
            pressed: "#263238",
            highlighted: "#3a464c",
            latched: "#6eaca8",
            active: "#6eaca8",
            text: "#e8eaed"
        },
        function: {
            normal: "#313c42",
            pressed: "#263238",
            highlighted: "#3a464c",
            text: "#e8eaed"
        },
        accent: {
            normal: "#6eaca8",
            pressed: "#5d918d",
            highlighted: "#6eaca8",
            latched: "#6eaca8",
            active: "#6eaca8",
            text: "#ffffff"
        },
        suggestions: {
            normal: "#313c42",
            pressed: "#263238",
            highlighted: "#3a464c",
            text: "#e8eaed"
        }
    })
}
