// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.plasma.keyboard.custom

QtObject {
    // Filled in by the style
    property real scaleHint

    readonly property string fontFamily: PlasmaKeyboardSettings.keyboardFontFamily.length > 0 ? PlasmaKeyboardSettings.keyboardFontFamily : Kirigami.Theme.defaultFont.family
    readonly property real keyBackgroundMargin: Math.round(8 * scaleHint)
    readonly property real keyContentMargin: Math.round(40 * scaleHint)
    readonly property real keyIconScale: scaleHint * 0.8

    property color primaryColor: Kirigami.Theme.backgroundColor
    property color primaryLightColor: Qt.lighter(primaryColor, 1.3)
    property color primaryDarkColor: Qt.darker(primaryColor, 1.3)
    property color textOnPrimaryColor: Kirigami.Theme.textColor
    property color secondaryColor: Kirigami.Theme.backgroundColor
    property color secondaryLightColor: Qt.lighter(secondaryColor, 1.3)
    property color secondaryDarkColor: Qt.darker(secondaryColor, 1.3)
    property color textOnSecondaryColor: Kirigami.Theme.textColor

    property color keyboardBackgroundColor: primaryColor
    property color normalKeyBackgroundColor: primaryLightColor
    property color normalKeyPressedBackgroundColor: primaryDarkColor
    property color highlightedKeyBackgroundColor: primaryLightColor
    property color latchedKeyBackgroundColor: Qt.lighter(normalKeyBackgroundColor, 1.6)
    property color capsLockKeyAccentColor: secondaryColor
    property color modeKeyAccentColor: textOnPrimaryColor
    property color keyTextColor: textOnPrimaryColor
    property color keySmallTextColor: textOnPrimaryColor
    property color popupBackgroundColor: secondaryColor
    property color popupBorderColor: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.textColor, secondaryColor, 0.9)
    property color popupTextColor: textOnSecondaryColor
    property color popupTextSelectedColor: textOnSecondaryColor
    property color popupHighlightBorderColor: Kirigami.Theme.highlightColor
    property color popupHighlightColor: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.3)
    property color selectionListTextColor: textOnPrimaryColor
    property color selectionListSeparatorColor: primaryLightColor
    property color selectionListBackgroundColor: primaryColor
    property color navigationHighlightColor: Qt.rgba(navigationHighlightBorderColor.r, navigationHighlightBorderColor.g, navigationHighlightBorderColor.b, 0.3)
    property color navigationHighlightBorderColor: Kirigami.Theme.highlightColor

    property string backgroundType: "color"
    property color backgroundStart: keyboardBackgroundColor
    property color backgroundEnd: backgroundStart
    property real backgroundAngle: 90

    property real keyOutlineWidth: 0
    property color keyOutlineColor: "transparent"
    property real keyShadowStrength: 1.0
    property string keyLabelCase: "normal"

    // Default per-category key colours. The "suggestions" entry keeps the
    // clipboard chips (a little lighter than a key, darker while pressed)
    // looking the same while still being themable.
    property var keyColors: ({})

    function setKeyColors(category, colors) {
        keyColors[category] = colors;
    }

    function setKeyColor(category, state, color) {
        if (!keyColors[category]) {
            keyColors[category] = ({});
        }
        keyColors[category][state] = color;
    }

    function keyColorFor(category, state) {
        if (category === "suggestions" && !(keyColors && keyColors["suggestions"])) {
            return state === "pressed" ? primaryDarkColor : Qt.lighter(normalKeyBackgroundColor, 1.5);
        }
        const categoryColors = keyColors ? keyColors[category] : undefined;
        if (categoryColors && categoryColors[state] !== undefined) {
            return categoryColors[state];
        }
        const normalColors = keyColors ? keyColors["normal"] : undefined;
        if (normalColors && normalColors[state] !== undefined) {
            return normalColors[state];
        }
        switch (state) {
        case "pressed":
            return normalKeyPressedBackgroundColor;
        case "highlighted":
            return highlightedKeyBackgroundColor;
        case "latched":
        case "active":
            return latchedKeyBackgroundColor;
        default:
            return normalKeyBackgroundColor;
        }
    }

    function keyTextColorFor(category) {
        const categoryColors = keyColors ? keyColors[category] : undefined;
        if (categoryColors && categoryColors.text !== undefined) {
            return categoryColors.text;
        }
        const normalColors = keyColors ? keyColors["normal"] : undefined;
        if (normalColors && normalColors.text !== undefined) {
            return normalColors.text;
        }
        return keyTextColor;
    }

    function keyOutlineFor(category) {
        const categoryColors = keyColors ? keyColors[category] : undefined;
        if (categoryColors) {
            if (categoryColors.outline !== undefined) {
                return {
                    width: categoryColors.outline.width !== undefined ? categoryColors.outline.width : keyOutlineWidth,
                    color: categoryColors.outline.color !== undefined ? categoryColors.outline.color : keyOutlineColor
                };
            }
            if (categoryColors.outlineWidth !== undefined || categoryColors.outlineColor !== undefined) {
                return {
                    width: categoryColors.outlineWidth !== undefined ? categoryColors.outlineWidth : keyOutlineWidth,
                    color: categoryColors.outlineColor !== undefined ? categoryColors.outlineColor : keyOutlineColor
                };
            }
        }
        return { width: keyOutlineWidth, color: keyOutlineColor };
    }

    function keyShadowFor(category) {
        const categoryColors = keyColors ? keyColors[category] : undefined;
        if (categoryColors && categoryColors.shadow !== undefined) {
            return categoryColors.shadow;
        }
        return keyShadowStrength;
    }

    function hasCategoryColors(category) {
        return keyColors !== undefined && keyColors[category] !== undefined;
    }

    function categoryActiveColorFor(category) {
        const categoryColors = keyColors ? keyColors[category] : undefined;
        if (categoryColors && categoryColors.active !== undefined) {
            return categoryColors.active;
        }
        return latchedKeyBackgroundColor;
    }

    readonly property real buttonRadius: Kirigami.Units.cornerRadius
    readonly property real popupRadius: Kirigami.Units.cornerRadius
}
