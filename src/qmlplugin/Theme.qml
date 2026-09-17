// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

pragma Singleton

import QtQuick
import QtQuick.VirtualKeyboard

QtObject {
    id: root

    property string themeId: "system"

    readonly property var availableThemes: [
        { id: "system", name: qsTr("System"), source: "" },
        { id: "light", name: qsTr("Light"), source: "" },
        { id: "dark", name: qsTr("Dark"), source: "" }
    ]

    property ThemePalette _current: ThemeSystemPalette {}
    readonly property ThemePalette current: _current

    readonly property Component systemPaletteComponent: Component { ThemeSystemPalette {} }
    readonly property Component lightPaletteComponent: Component { ThemeLightPalette {} }
    readonly property Component darkPaletteComponent: Component { ThemeDarkPalette {} }

    function setThemeId(id) {
        let component = systemPaletteComponent;
        if (id === "light") {
            component = lightPaletteComponent;
        } else if (id === "dark") {
            component = darkPaletteComponent;
        }
        themeId = id;
        const previous = _current;
        _current = component.createObject(root);
        if (previous) {
            previous.destroy();
        }
    }

    function categoryOf(keyItem) {
        if (!keyItem) {
            return "normal";
        }
        if (keyItem.__themeCategory) {
            return keyItem.__themeCategory;
        }
        if (keyItem.keyType === QtVirtualKeyboard.KeyType.ShiftKey
                || [Qt.Key_Control, Qt.Key_Alt, Qt.Key_Shift, Qt.Key_AltGr, Qt.Key_Meta, Qt.Key_CapsLock].indexOf(keyItem.key) !== -1) {
            return "modifier";
        }
        if (keyItem.functionKey === true || keyItem.key === Qt.Key_Space) {
            return "function";
        }
        if (typeof keyItem.text === "string" && keyItem.text.length === 1 && /^[^a-zа-яё]$/i.test(keyItem.text)) {
            return "digit";
        }
        return "normal";
    }

    function keyColor(category, state) {
        return current.keyColorFor(category, state);
    }
}
