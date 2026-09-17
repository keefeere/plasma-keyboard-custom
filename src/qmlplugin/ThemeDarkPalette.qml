// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

ThemePalette {
    normalKeyBackgroundColor: Qt.lighter(primaryLightColor, 1.35)
    normalKeyPressedBackgroundColor: Qt.lighter(primaryDarkColor, 1.1)
    keyboardBackgroundColor: primaryColor

    keyColors: ({
        modifier: {
            normal: Qt.darker(normalKeyBackgroundColor, 1.25),
            pressed: Qt.darker(normalKeyPressedBackgroundColor, 1.15),
            text: keyTextColor
        }
    })
}
