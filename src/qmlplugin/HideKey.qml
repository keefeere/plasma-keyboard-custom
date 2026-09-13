// SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components

/**
 * Hide key that hides the actual keyboard window.
 *
 * Qt Virtual Keyboard's HideKeyboardKey only hides its internal panel, which
 * is not enough for the Wayland input-panel window. Calling
 * QInputMethod::hide() makes the window hide as well.
 */
BaseKey {
    keyType: QtVirtualKeyboard.KeyType.HideKeyboardKey
    functionKey: true
    highlighted: true
    keyPanelDelegate: keyboard.style ? keyboard.style.hideKeyPanel : undefined
    onClicked: Qt.inputMethod.hide()
}
