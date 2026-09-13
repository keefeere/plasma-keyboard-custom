// SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.VirtualKeyboard.Components

/**
 * A latching Ctrl/Alt modifier key.
 *
 * Tapping it enables the modifier for the next key press, after which it is
 * cleared automatically. Tapping it again turns it off.
 */
Key {
    id: root

    /**
     * Which modifier this key controls: "ctrl" or "alt".
     */
    property string modifier: "ctrl"

    noKeyEvent: true
    functionKey: true
    highlighted: root.modifier === "alt" ? Modifiers.alt : Modifiers.ctrl

    onClicked: {
        if (root.modifier === "alt") {
            Modifiers.alt = !Modifiers.alt;
        } else {
            Modifiers.ctrl = !Modifiers.ctrl;
        }
    }
}
