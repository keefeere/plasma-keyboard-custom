/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.AbstractKCM {
    id: root

    readonly property var currentTheme: kcm.availableThemes[themeComboBox.currentIndex] || ({})

    header: QQC2.TabBar {
        id: tabBar

        QQC2.TabButton {
            text: i18n("Layouts")
        }

        QQC2.TabButton {
            text: i18nc("@title:tab", "Opening")
        }

        QQC2.TabButton {
            text: i18n("Appearance")
        }

        QQC2.TabButton {
            text: i18n("Typing")
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        LocaleSelectorListView {
            id: list
        }

        SettingsFormPage {
            QQC2.CheckBox {
                id: showOnLongTap
                Kirigami.FormData.label: i18n("Open on long press:")
                text: i18n("Hold a finger on a text field")

                checked: kcm.showOnLongTap
                onCheckedChanged: {
                    kcm.showOnLongTap = checked;
                    checked = Qt.binding(() => kcm.showOnLongTap);
                }
            }

            QQC2.SpinBox {
                id: showOnLongTapThreshold
                Kirigami.FormData.label: i18n("Long press delay:")
                from: 100
                to: 5000
                stepSize: 100
                editable: true
                enabled: showOnLongTap.checked

                value: kcm.showOnLongTapThresholdMs
                onValueModified: {
                    kcm.showOnLongTapThresholdMs = value;
                    value = Qt.binding(() => kcm.showOnLongTapThresholdMs);
                }

                textFromValue: function (value) {
                    return i18nc("duration in milliseconds", "%1 ms", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.showOnLongTapThresholdMs : number;
                }
            }

            QQC2.CheckBox {
                id: showOnMouseFocus
                Kirigami.FormData.label: i18n("Open on focus:")
                text: i18n("When a text field is focused with a mouse")

                checked: kcm.showOnMouseFocus
                onCheckedChanged: {
                    kcm.showOnMouseFocus = checked;
                    checked = Qt.binding(() => kcm.showOnMouseFocus);
                }
            }

            QQC2.CheckBox {
                id: hidePanelWhenKeyboardVisible
                text: i18n("Hide the panel while the keyboard is open")

                checked: kcm.hidePanelWhenKeyboardVisible
                onCheckedChanged: {
                    kcm.hidePanelWhenKeyboardVisible = checked;
                    checked = Qt.binding(() => kcm.hidePanelWhenKeyboardVisible);
                }
            }
        }

        SettingsFormPage {
            QQC2.SpinBox {
                id: keyboardHeightSpinBox
                Kirigami.FormData.label: i18n("Keyboard height:")
                from: 20
                to: 80
                stepSize: 2
                value: kcm.keyboardHeightPercent

                textFromValue: function (value) {
                    return i18nc("keyboard height in percent", "%1%", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.keyboardHeightPercent : number;
                }

                onValueChanged: {
                    kcm.keyboardHeightPercent = value;
                    value = Qt.binding(() => kcm.keyboardHeightPercent);
                }
            }

            QQC2.SpinBox {
                id: floatingKeyboardWidthSpinBox
                Kirigami.FormData.label: i18n("Floating keyboard width:")
                from: 20
                to: 100
                stepSize: 5
                value: kcm.floatingKeyboardWidthPercent

                textFromValue: function (value) {
                    return i18nc("keyboard width in percent", "%1%", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.floatingKeyboardWidthPercent : number;
                }

                onValueChanged: {
                    kcm.floatingKeyboardWidthPercent = value;
                    value = Qt.binding(() => kcm.floatingKeyboardWidthPercent);
                }
            }

            QQC2.SpinBox {
                id: floatingKeyboardOpacitySpinBox
                Kirigami.FormData.label: i18n("Floating keyboard opacity:")
                from: 20
                to: 100
                stepSize: 5
                value: kcm.floatingKeyboardOpacity

                textFromValue: function (value) {
                    return i18nc("keyboard opacity in percent", "%1%", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.floatingKeyboardOpacity : number;
                }

                onValueChanged: {
                    kcm.floatingKeyboardOpacity = value;
                    value = Qt.binding(() => kcm.floatingKeyboardOpacity);
                }
            }

            QQC2.ComboBox {
                id: keyboardFontComboBox
                Kirigami.FormData.label: i18n("Keyboard font:")
                Layout.preferredWidth: Kirigami.Units.gridUnit * 16

                model: [i18n("Default")].concat(Qt.fontFamilies())
                currentIndex: Math.max(0, model.indexOf(kcm.keyboardFontFamily))

                onActivated: (index) => {
                    kcm.keyboardFontFamily = index === 0 ? "" : model[index];
                }
            }

            QQC2.ComboBox {
                id: themeComboBox
                Kirigami.FormData.label: i18n("Theme:")
                Layout.preferredWidth: Kirigami.Units.gridUnit * 16

                model: kcm.availableThemes
                textRole: "name"
                valueRole: "id"
                currentIndex: Math.max(0, model.findIndex(theme => theme.id === kcm.theme))

                onActivated: (index) => {
                    kcm.theme = model[index].id;
                }
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Theme files:")

                QQC2.Button {
                    text: i18n("Import theme…")
                    onClicked: {
                        themeError.text = "";
                        importThemeDialog.open();
                    }
                }

                QQC2.Button {
                    text: i18n("Export theme…")
                    onClicked: {
                        themeError.text = "";
                        exportThemeDialog.themeId = themeComboBox.currentValue;
                        exportThemeDialog.open();
                    }
                }

                QQC2.Button {
                    text: i18n("Remove theme")
                    enabled: root.currentTheme.source === "user"
                    onClicked: {
                        themeError.text = "";
                        removeThemeDialog.open();
                    }
                }
            }

            Kirigami.InlineMessage {
                id: themeError
                Layout.fillWidth: true
                type: Kirigami.MessageType.Error
                visible: text.length > 0
            }

            QQC2.CheckBox {
                id: showFunctionKeyRow
                Kirigami.FormData.label: i18n("Function keys:")
                text: i18n("Show an F1–F12 row above the keyboard")

                checked: kcm.showFunctionKeyRow
                onCheckedChanged: {
                    kcm.showFunctionKeyRow = checked;
                    checked = Qt.binding(() => kcm.showFunctionKeyRow);
                }
            }

            QQC2.CheckBox {
                id: clipboardEnabled
                Kirigami.FormData.label: i18n("Clipboard:")
                text: i18n("Show recent clipboard entries above the keyboard")

                checked: kcm.clipboardEnabled
                onCheckedChanged: {
                    kcm.clipboardEnabled = checked;
                    checked = Qt.binding(() => kcm.clipboardEnabled);
                }
            }
        }

        SettingsFormPage {
            QQC2.CheckBox {
                id: autoCapitalizationEnabled
                Kirigami.FormData.label: i18n("Automatic capitalization:")
                text: i18n("Capitalize the first letter of a sentence")

                checked: kcm.autoCapitalizationEnabled
                onCheckedChanged: {
                    kcm.autoCapitalizationEnabled = checked;
                    checked = Qt.binding(() => kcm.autoCapitalizationEnabled);
                }
            }

            QQC2.CheckBox {
                id: diacriticsCheckbox
                Kirigami.FormData.label: i18n("Alternate characters:")
                text: i18n("Show a popup when holding a key")

                checked: kcm.diacriticsPopupEnabled
                onCheckedChanged: {
                    kcm.diacriticsPopupEnabled = checked;
                    checked = Qt.binding(() => kcm.diacriticsPopupEnabled);
                }
            }

            QQC2.SpinBox {
                id: diacriticsDelaySpinBox
                Kirigami.FormData.label: i18n("Hold delay:")
                from: 100
                to: 1500
                stepSize: 50
                enabled: diacriticsCheckbox.checked
                value: kcm.diacriticsHoldThresholdMs

                textFromValue: function (value) {
                    return i18nc("duration in milliseconds", "%1 ms", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.diacriticsHoldThresholdMs : number;
                }

                onValueChanged: {
                    kcm.diacriticsHoldThresholdMs = value;
                    value = Qt.binding(() => kcm.diacriticsHoldThresholdMs);
                }
            }

            QQC2.CheckBox {
                id: gamepadAlternatesCheckbox
                Kirigami.FormData.label: i18n("Gamepad:")
                text: i18n("Hold the A button to pick an alternate character")

                checked: kcm.gamepadAlternatesEnabled
                onCheckedChanged: {
                    kcm.gamepadAlternatesEnabled = checked;
                    checked = Qt.binding(() => kcm.gamepadAlternatesEnabled);
                }
            }

            QQC2.SpinBox {
                id: gamepadAlternatesDelaySpinBox
                Kirigami.FormData.label: i18n("Gamepad hold delay:")
                from: 100
                to: 1500
                stepSize: 50
                enabled: gamepadAlternatesCheckbox.checked
                value: kcm.gamepadAlternatesThresholdMs

                textFromValue: function (value) {
                    return i18nc("duration in milliseconds", "%1 ms", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.gamepadAlternatesThresholdMs : number;
                }

                onValueChanged: {
                    kcm.gamepadAlternatesThresholdMs = value;
                    value = Qt.binding(() => kcm.gamepadAlternatesThresholdMs);
                }
            }

            QQC2.CheckBox {
                id: predictiveTextCheckbox
                Kirigami.FormData.label: i18n("Word suggestions:")
                text: i18n("Offer words that continue what is being typed")

                checked: kcm.predictiveTextEnabled
                onCheckedChanged: {
                    kcm.predictiveTextEnabled = checked;
                    checked = Qt.binding(() => kcm.predictiveTextEnabled);
                }
            }

            QQC2.SpinBox {
                id: predictiveSuggestionCountSpinBox
                Kirigami.FormData.label: i18n("Suggestions at once:")
                from: 1
                to: 5
                enabled: predictiveTextCheckbox.checked
                value: kcm.predictiveSuggestionCount

                onValueChanged: {
                    kcm.predictiveSuggestionCount = value;
                    value = Qt.binding(() => kcm.predictiveSuggestionCount);
                }
            }

            QQC2.SpinBox {
                id: predictiveMinPrefixLengthSpinBox
                Kirigami.FormData.label: i18n("Letters before suggesting:")
                from: 1
                to: 4
                enabled: predictiveTextCheckbox.checked
                value: kcm.predictiveMinPrefixLength

                textFromValue: function (value) {
                    return i18np("%1 letter", "%1 letters", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.predictiveMinPrefixLength : number;
                }

                onValueChanged: {
                    kcm.predictiveMinPrefixLength = value;
                    value = Qt.binding(() => kcm.predictiveMinPrefixLength);
                }
            }

            QQC2.CheckBox {
                id: predictiveNextWordCheckbox
                text: i18n("Offer words that may follow the typed one")

                enabled: predictiveTextCheckbox.checked
                checked: kcm.predictiveNextWordEnabled
                onCheckedChanged: {
                    kcm.predictiveNextWordEnabled = checked;
                    checked = Qt.binding(() => kcm.predictiveNextWordEnabled);
                }
            }

            QQC2.CheckBox {
                id: predictiveTypoCorrectionCheckbox
                text: i18n("Offer to correct a mistyped word")

                enabled: predictiveTextCheckbox.checked
                checked: kcm.predictiveTypoCorrectionEnabled
                onCheckedChanged: {
                    kcm.predictiveTypoCorrectionEnabled = checked;
                    checked = Qt.binding(() => kcm.predictiveTypoCorrectionEnabled);
                }
            }

            QQC2.CheckBox {
                id: soundsEnabled
                Kirigami.FormData.label: i18n("Key press feedback:")
                text: i18n("Sound")

                checked: kcm.soundEnabled
                onCheckedChanged: {
                    kcm.soundEnabled = checked;
                    checked = Qt.binding(() => kcm.soundEnabled);
                }
            }

            QQC2.CheckBox {
                id: vibrationEnabled
                text: i18n("Vibration")

                checked: kcm.vibrationEnabled
                onCheckedChanged: {
                    kcm.vibrationEnabled = checked;
                    checked = Qt.binding(() => kcm.vibrationEnabled);
                }
            }

            QQC2.SpinBox {
                id: vibrationStrengthSpinBox
                Kirigami.FormData.label: i18n("Vibration strength:")
                from: 0
                to: 100
                stepSize: 5
                enabled: vibrationEnabled.checked
                value: kcm.vibrationStrength

                textFromValue: function (value) {
                    return i18nc("vibration strength in percent", "%1%", value);
                }
                valueFromText: function (text) {
                    const number = parseInt(text);
                    return isNaN(number) ? kcm.vibrationStrength : number;
                }

                onValueChanged: {
                    kcm.vibrationStrength = value;
                    value = Qt.binding(() => kcm.vibrationStrength);
                }
            }

            QQC2.CheckBox {
                id: keyboardNavigationEnabled
                Kirigami.FormData.label: i18n("Navigation:")
                text: i18n("Arrow keys move between the keys")

                checked: kcm.keyboardNavigationEnabled
                onCheckedChanged: {
                    kcm.keyboardNavigationEnabled = checked;
                    checked = Qt.binding(() => kcm.keyboardNavigationEnabled);
                }
            }

            QQC2.TextField {
                id: testField
                Kirigami.FormData.label: i18n("Try it:")
                Layout.preferredWidth: Kirigami.Units.gridUnit * 20

                placeholderText: i18n("Type here to see the keyboard")
            }
        }
    }

    FileDialog {
        id: importThemeDialog
        title: i18n("Import theme")
        fileMode: FileDialog.OpenFile
        nameFilters: [i18n("Theme files (*.json)"), i18n("All files (*)")]

        onAccepted: {
            const error = kcm.installTheme(selectedFile);
            if (error.length > 0) {
                themeError.text = i18n("Could not import the theme: %1", error);
            }
        }
    }

    FileDialog {
        id: exportThemeDialog
        property string themeId
        title: i18n("Export theme")
        fileMode: FileDialog.SaveFile
        defaultSuffix: "json"
        nameFilters: [i18n("Theme files (*.json)"), i18n("All files (*)")]

        onAccepted: {
            const error = kcm.exportTheme(themeId, selectedFile);
            if (error.length > 0) {
                themeError.text = i18n("Could not export the theme: %1", error);
            }
        }
    }

    Kirigami.PromptDialog {
        id: removeThemeDialog
        title: i18n("Remove theme?")
        subtitle: i18n("The theme \"%1\" will be deleted permanently.", root.currentTheme.name || "")
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        onAccepted: {
            const error = kcm.removeUserTheme(root.currentTheme.id);
            if (error.length > 0) {
                themeError.text = i18n("Could not remove the theme: %1", error);
            }
        }
    }
}
