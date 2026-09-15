/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kcmutils as KCM
import org.kde.kirigami as Kirigami
import org.kde.bigscreen as Bigscreen

KCM.SimpleKCM {
    id: keyboardSettingsView

    title: i18n("On-Screen Keyboard")
    background: null

    leftPadding: Kirigami.Units.smallSpacing
    topPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing

    onActiveFocusChanged: {
        if (activeFocus) {
            changeLanguagesButton.forceActiveFocus()
        }
    }

    ColumnLayout {
        id: column
        KeyNavigation.left: keyboardSettingsView.KeyNavigation.left
        spacing: 0

        Bigscreen.ButtonDelegate {
            id: changeLanguagesButton
            text: i18n("Languages")
            onClicked: localeSelectorSidebar.open()
            KeyNavigation.down: soundOnKeypressButton
        }

        QQC2.Label {
            id: keyPressFeedbackLabel
            text: i18n("Feedback")
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Bigscreen.SwitchDelegate {
            id: soundOnKeypressButton
            text: i18n("Sound")
            KeyNavigation.up: changeLanguagesButton
            KeyNavigation.down: vibrationOnKeypressButton

            checked: kcm.soundEnabled
            onCheckedChanged: {
                kcm.soundEnabled = checked;
                checked = Qt.binding(() => kcm.soundEnabled);
            }
        }

        Bigscreen.SwitchDelegate {
            id: vibrationOnKeypressButton
            text: i18n("Vibration")
            KeyNavigation.up: soundOnKeypressButton
            KeyNavigation.down: autoCapitalizationButton

            checked: kcm.vibrationEnabled
            onCheckedChanged: {
                kcm.vibrationEnabled = checked;
                checked = Qt.binding(() => kcm.vibrationEnabled);
            }
        }

        QQC2.Label {
            id: generalLabel
            text: i18n("Opening the keyboard")
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Bigscreen.SwitchDelegate {
            id: showOnLongTapButton
            text: i18n("Open on long press")
            KeyNavigation.up: vibrationOnKeypressButton
            KeyNavigation.down: showOnMouseFocusButton

            checked: kcm.showOnLongTap
            onCheckedChanged: {
                kcm.showOnLongTap = checked;
                checked = Qt.binding(() => kcm.showOnLongTap);
            }
        }

        Bigscreen.SwitchDelegate {
            id: showOnMouseFocusButton
            text: i18n("Open when focused with a mouse")
            KeyNavigation.up: showOnLongTapButton
            KeyNavigation.down: hidePanelWhenKeyboardVisibleButton

            checked: kcm.showOnMouseFocus
            onCheckedChanged: {
                kcm.showOnMouseFocus = checked;
                checked = Qt.binding(() => kcm.showOnMouseFocus);
            }
        }

        Bigscreen.SwitchDelegate {
            id: hidePanelWhenKeyboardVisibleButton
            text: i18n("Hide the panel while the keyboard is open")
            KeyNavigation.up: showOnMouseFocusButton
            KeyNavigation.down: showFunctionKeyRowButton

            checked: kcm.hidePanelWhenKeyboardVisible
            onCheckedChanged: {
                kcm.hidePanelWhenKeyboardVisible = checked;
                checked = Qt.binding(() => kcm.hidePanelWhenKeyboardVisible);
            }
        }

        QQC2.Label {
            id: appearanceLabel
            text: i18n("Appearance")
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Bigscreen.SwitchDelegate {
            id: showFunctionKeyRowButton
            text: i18n("Function keys")
            KeyNavigation.up: hidePanelWhenKeyboardVisibleButton
            KeyNavigation.down: autoCapitalizationButton

            checked: kcm.showFunctionKeyRow
            onCheckedChanged: {
                kcm.showFunctionKeyRow = checked;
                checked = Qt.binding(() => kcm.showFunctionKeyRow);
            }
        }

        QQC2.ComboBox {
            id: keyboardFontComboBox
            Layout.preferredWidth: column.width
            KeyNavigation.up: showFunctionKeyRowButton
            KeyNavigation.down: autoCapitalizationButton

            model: [i18n("Default")].concat(Qt.fontFamilies())
            currentIndex: Math.max(0, model.indexOf(kcm.keyboardFontFamily))

            onActivated: (index) => {
                kcm.keyboardFontFamily = index === 0 ? "" : model[index];
            }
        }

        QQC2.Label {
            id: typingLabel
            text: i18n("Typing")
            font.pixelSize: 22
            font.weight: Font.Normal
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Bigscreen.SwitchDelegate {
            id: autoCapitalizationButton
            text: i18n("Automatic capitalization")
            KeyNavigation.up: keyboardFontComboBox
            KeyNavigation.down: diacriticsButton

            checked: kcm.autoCapitalizationEnabled
            onCheckedChanged: {
                kcm.autoCapitalizationEnabled = checked;
                checked = Qt.binding(() => kcm.autoCapitalizationEnabled);
            }
        }

        Bigscreen.SwitchDelegate {
            id: diacriticsButton
            text: i18n("Alternate characters")
            KeyNavigation.up: autoCapitalizationButton

            checked: kcm.diacriticsPopupEnabled
            onCheckedChanged: {
                kcm.diacriticsPopupEnabled = checked;
                checked = Qt.binding(() => kcm.diacriticsPopupEnabled);
            }
        }

        LocaleSelectorSidebar {
            id: localeSelectorSidebar
            onClosed: changeLanguagesButton.forceActiveFocus()
        }
    }
}
