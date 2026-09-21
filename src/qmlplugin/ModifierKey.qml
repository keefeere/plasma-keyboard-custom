// SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components

/**
 * A latching Ctrl/Alt/Meta modifier key.
 *
 * It emits a plain Control/Alt/Meta key event; InputListenerItem turns that
 * into a latch toggle, so it works the same for touch and gamepad activation.
 * Tap to enable, tap again (or press a key, which completes the combination)
 * to clear. The latched state is shown by a lighter key background.
 */
Key {
    id: root

    /** Which modifier this key controls: "ctrl", "alt" or "meta". */
    property string modifier: "ctrl"

    /** True while the modifier is latched; the key panel uses it for its color. */
    readonly property bool latched: {
        if (root.modifier === "alt") return Modifiers.alt;
        if (root.modifier === "meta") return Modifiers.meta;
        return Modifiers.ctrl;
    }

    key: {
        if (root.modifier === "alt") return Qt.Key_Alt;
        if (root.modifier === "meta") return Qt.Key_Meta;
        return Qt.Key_Control;
    }
    functionKey: true
    noModifier: true
}
