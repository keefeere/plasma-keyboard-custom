// SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components
import QtQuick.VirtualKeyboard.Settings

/**
 * Language key with a one-click switch and a long-press popup.
 *
 * A normal tap cycles to the next enabled keyboard layout. Holding the key
 * opens the language popup (which also offers the keyboard settings).
 */
BaseKey {
    id: root

    /*! If true, only switch between languages that provide a custom layout. */
    property bool customLayoutsOnly: false

    /*! How long the key must be held (ms) before the popup opens. */
    property int longPressInterval: 500

    keyType: QtVirtualKeyboard.KeyType.ChangeLanguageKey
    objectName: "changeLanguageKey"
    functionKey: true
    highlighted: true
    displayText: keyboard.locale.split("_")[0]
    keyPanelDelegate: keyboard.style ? keyboard.style.languageKeyPanel : undefined
    visible: VirtualKeyboardSettings.visibleFunctionKeys & QtVirtualKeyboard.KeyboardFunctionKeys.Language
    enabled: keyboard.isKeyboardFunctionAvailable(QtVirtualKeyboard.KeyboardFunction.ChangeLanguage, customLayoutsOnly)

    property bool longPressTriggered: false

    Timer {
        id: longPressTimer
        interval: root.longPressInterval
        onTriggered: {
            root.longPressTriggered = true;
            keyboard.showLanguagePopup(root, root.customLayoutsOnly);
        }
    }

    onPressedChanged: {
        if (pressed) {
            root.longPressTriggered = false;
            longPressTimer.start();
        } else {
            longPressTimer.stop();
        }
    }

    onClicked: {
        if (root.longPressTriggered) {
            return;
        }
        keyboard.changeInputLanguage(root.customLayoutsOnly);
    }
}
