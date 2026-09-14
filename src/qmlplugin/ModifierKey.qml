// SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components

/**
 * A latching Ctrl/Alt modifier key.
 *
 * It emits a plain Control/Alt key event; InputListenerItem turns that into a
 * latch toggle, so it works the same for touch and gamepad activation. Tap to
 * enable, tap again (or press a key, which completes the combination) to clear.
 * The latched state is shown by highlighting the key and a dot in the corner.
 */
Key {
    id: root

    /** Which modifier this key controls: "ctrl" or "alt". */
    property string modifier: "ctrl"

    /** True while the modifier is latched. */
    readonly property bool latched: root.modifier === "alt" ? Modifiers.alt : Modifiers.ctrl

    key: root.modifier === "alt" ? Qt.Key_Alt : Qt.Key_Control
    functionKey: true
    noModifier: true
    highlighted: root.latched
    smallText: root.latched ? "\u25cf" : ""
    smallTextVisible: root.latched
}
