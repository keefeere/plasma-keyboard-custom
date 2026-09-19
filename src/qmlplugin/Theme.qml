// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

pragma Singleton

import QtQuick
import QtQuick.VirtualKeyboard

QtObject {
    id: root

    property string themeId: "system"

    // Built-in themes plus the user themes found in
    // ~/.local/share/plasma-keyboard/themes. Each entry is
    // { id, name, source, base } with source builtin|user.
    readonly property var availableThemes: ThemeManager.availableThemes

    property ThemePalette _current: ThemeSystemPalette {}
    readonly property ThemePalette current: _current

    readonly property Component systemPaletteComponent: Component { ThemeSystemPalette {} }
    readonly property Component lightPaletteComponent: Component { ThemeLightPalette {} }
    readonly property Component darkPaletteComponent: Component { ThemeDarkPalette {} }
    readonly property Component iosLightPaletteComponent: Component { ThemeIosLightPalette {} }
    readonly property Component iosDarkPaletteComponent: Component { ThemeIosDarkPalette {} }
    readonly property Component materialLightPaletteComponent: Component { ThemeMaterialLightPalette {} }
    readonly property Component materialDarkPaletteComponent: Component { ThemeMaterialDarkPalette {} }
    readonly property Component catppuccinMochaPaletteComponent: Component { ThemeCatppuccinMochaPalette {} }

    // A theme was installed/removed/renamed. If the selected theme is gone
    // (for instance its file was deleted) fall back to the system palette.
    function handleThemesChanged() {
        if (!themeEntry(themeId)) {
            setThemeId("system");
        }
    }

    Component.onCompleted: ThemeManager.themesChanged.connect(handleThemesChanged)

    // The entry of @p id, or undefined when it is unknown.
    function themeEntry(id) {
        const themes = ThemeManager.availableThemes;
        for (let i = 0; i < themes.length; ++i) {
            if (themes[i].id === id) {
                return themes[i];
            }
        }
        return undefined;
    }

    // The inline palette component of a built-in id, "system" for anything else.
    function paletteComponentFor(id) {
        if (id === "light") {
            return lightPaletteComponent;
        } else if (id === "dark") {
            return darkPaletteComponent;
        } else if (id === "ios-light") {
            return iosLightPaletteComponent;
        } else if (id === "ios-dark") {
            return iosDarkPaletteComponent;
        } else if (id === "material-light") {
            return materialLightPaletteComponent;
        } else if (id === "material-dark") {
            return materialDarkPaletteComponent;
        } else if (id === "catppuccin-mocha") {
            return catppuccinMochaPaletteComponent;
        }
        return systemPaletteComponent;
    }

    // Applies a raw user theme description on top of a base palette. The
    // palette/geometry keys keep the palette property names, the background
    // and keyStyle sections use the shorter JSON names.
    function applyThemeDescription(palette, description) {
        const paletteValues = description.palette;
        if (paletteValues) {
            for (const key in paletteValues) {
                palette[key] = paletteValues[key];
            }
        }

        const geometry = description.geometry;
        if (geometry) {
            for (const key in geometry) {
                palette[key] = geometry[key];
            }
        }

        const background = description.background;
        if (background) {
            if (background.type !== undefined) {
                palette.backgroundType = background.type;
            }
            if (background.start !== undefined) {
                palette.backgroundStart = background.start;
            }
            if (background.end !== undefined) {
                palette.backgroundEnd = background.end;
            }
            if (background.angle !== undefined) {
                palette.backgroundAngle = background.angle;
            }
        }

        const keyStyle = description.keyStyle;
        if (keyStyle) {
            if (keyStyle.outlineWidth !== undefined) {
                palette.keyOutlineWidth = keyStyle.outlineWidth;
            }
            if (keyStyle.outlineColor !== undefined) {
                palette.keyOutlineColor = keyStyle.outlineColor;
            }
            if (keyStyle.shadowStrength !== undefined) {
                palette.keyShadowStrength = keyStyle.shadowStrength;
            }
            if (keyStyle.labelCase !== undefined) {
                palette.keyLabelCase = keyStyle.labelCase;
            }
        }

        // Replace the whole map at once; the shapes (category -> state ->
        // colour, plus text/outline/shadow) are what keyColorFor and
        // keyTextColorFor read.
        if (description.keyColors !== undefined) {
            palette.keyColors = description.keyColors;
        }
    }

    function setThemeId(id) {
        let entry = themeEntry(id);
        if (!entry) {
            id = "system";
            entry = themeEntry("system");
        }
        if (!entry) {
            // The built-in list is always present; this only guards against a
            // broken registration.
            entry = { id: "system", name: "System", source: "builtin", base: "system" };
        }

        let description;
        if (entry.source === "user") {
            description = ThemeManager.themeDescription(id);
            if (!description || Object.keys(description).length === 0) {
                // The file disappeared (for instance the selected user theme
                // was just deleted): fall back to the system palette.
                id = "system";
                entry = { id: "system", name: "System", source: "builtin", base: "system" };
                description = undefined;
            }
        }

        const component = paletteComponentFor(entry.source === "user" && description ? entry.base : id);
        themeId = id;
        const previous = _current;
        _current = component.createObject(root);
        if (entry.source === "user" && description) {
            applyThemeDescription(_current, description);
        }
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
        if (keyItem.keyType === QtVirtualKeyboard.KeyType.EnterKey) {
            return "accent";
        }
        if (keyItem.keyType === QtVirtualKeyboard.KeyType.ShiftKey
                || [Qt.Key_Control, Qt.Key_Alt, Qt.Key_Shift, Qt.Key_AltGr, Qt.Key_Meta, Qt.Key_CapsLock].indexOf(keyItem.key) !== -1) {
            return "modifier";
        }
        // The space bar reads as a normal key, so a theme can tint the special
        // keys (backspace, mode, ...) without tinting the space bar.
        if (keyItem.keyType === QtVirtualKeyboard.KeyType.SpaceKey || keyItem.key === Qt.Key_Space) {
            return "normal";
        }
        if (keyItem.functionKey === true) {
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
