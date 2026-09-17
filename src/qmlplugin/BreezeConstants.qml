// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

pragma Singleton

QtObject {
    readonly property ThemePalette _theme: Theme.current

    // Filled in by the style
    readonly property real scaleHint: _theme.scaleHint

    readonly property string fontFamily: _theme.fontFamily
    readonly property real keyBackgroundMargin: _theme.keyBackgroundMargin
    readonly property real keyContentMargin: _theme.keyContentMargin
    readonly property real keyIconScale: _theme.keyIconScale

    readonly property color primaryColor: _theme.primaryColor
    readonly property color primaryLightColor: _theme.primaryLightColor
    readonly property color primaryDarkColor: _theme.primaryDarkColor
    readonly property color textOnPrimaryColor: _theme.textOnPrimaryColor
    readonly property color secondaryColor: _theme.secondaryColor
    readonly property color secondaryLightColor: _theme.secondaryLightColor
    readonly property color secondaryDarkColor: _theme.secondaryDarkColor
    readonly property color textOnSecondaryColor: _theme.textOnSecondaryColor

    readonly property color keyboardBackgroundColor: _theme.keyboardBackgroundColor
    readonly property color normalKeyBackgroundColor: _theme.normalKeyBackgroundColor
    readonly property color normalKeyPressedBackgroundColor: _theme.normalKeyPressedBackgroundColor
    readonly property color highlightedKeyBackgroundColor: _theme.highlightedKeyBackgroundColor
    readonly property color latchedKeyBackgroundColor: _theme.latchedKeyBackgroundColor
    readonly property color capsLockKeyAccentColor: _theme.capsLockKeyAccentColor
    readonly property color modeKeyAccentColor: _theme.modeKeyAccentColor
    readonly property color keyTextColor: _theme.keyTextColor
    readonly property color keySmallTextColor: _theme.keySmallTextColor
    readonly property color popupBackgroundColor: _theme.popupBackgroundColor
    readonly property color popupBorderColor: _theme.popupBorderColor
    readonly property color popupTextColor: _theme.popupTextColor
    readonly property color popupTextSelectedColor: _theme.popupTextSelectedColor
    readonly property color popupHighlightBorderColor: _theme.popupHighlightBorderColor
    readonly property color popupHighlightColor: _theme.popupHighlightColor
    readonly property color selectionListTextColor: _theme.selectionListTextColor
    readonly property color selectionListSeparatorColor: _theme.selectionListSeparatorColor
    readonly property color selectionListBackgroundColor: _theme.selectionListBackgroundColor
    readonly property color navigationHighlightColor: _theme.navigationHighlightColor
    readonly property color navigationHighlightBorderColor: _theme.navigationHighlightBorderColor

    readonly property string backgroundType: _theme.backgroundType
    readonly property color backgroundStart: _theme.backgroundStart
    readonly property color backgroundEnd: _theme.backgroundEnd
    readonly property real backgroundAngle: _theme.backgroundAngle

    readonly property real keyOutlineWidth: _theme.keyOutlineWidth
    readonly property color keyOutlineColor: _theme.keyOutlineColor
    readonly property real keyShadowStrength: _theme.keyShadowStrength
    readonly property string keyLabelCase: _theme.keyLabelCase

    readonly property var keyColors: _theme.keyColors

    readonly property real buttonRadius: _theme.buttonRadius
    readonly property real popupRadius: _theme.popupRadius
}
