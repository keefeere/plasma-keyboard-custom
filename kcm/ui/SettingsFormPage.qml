/*
    SPDX-FileCopyrightText: 2026 plasma-keyboard-custom contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

/**
 * Scrollable page holding a Kirigami.FormLayout, used as a tab page in the
 * settings KCM.
 */
QQC2.ScrollView {
    id: page

    default property alias content: formLayout.data

    contentWidth: availableWidth
    clip: true

    Kirigami.FormLayout {
        id: formLayout
        width: page.availableWidth
    }
}
