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

KCM.ScrollViewKCM {
    id: root

    view: LocaleSelectorListView {
        id: list

        Kirigami.Separator {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }

    footer: Kirigami.FormLayout {
        id: formLayout

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

        QQC2.CheckBox {
            id: keyboardNavigationEnabled
            Kirigami.FormData.label: i18n("General:")
            text: i18n("Keyboard navigation")

            checked: kcm.keyboardNavigationEnabled
            onCheckedChanged: {
                kcm.keyboardNavigationEnabled = checked;
                checked = Qt.binding(() => kcm.keyboardNavigationEnabled);
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
            text: i18n("Hide the panel while the keyboard is visible")

            checked: kcm.hidePanelWhenKeyboardVisible
            onCheckedChanged: {
                kcm.hidePanelWhenKeyboardVisible = checked;
                checked = Qt.binding(() => kcm.hidePanelWhenKeyboardVisible);
            }
        }

        QQC2.CheckBox {
            id: autoCapitalizationEnabled
            text: i18n("Auto-capitalization")

            checked: kcm.autoCapitalizationEnabled
            onCheckedChanged: {
                kcm.autoCapitalizationEnabled = checked;
                checked = Qt.binding(() => kcm.autoCapitalizationEnabled);
            }
        }

        QQC2.CheckBox {
            id: diacriticsCheckbox
            Kirigami.FormData.label: i18n("Alternate characters:")
            text: i18n("Show popup when holding a key")

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

            // Include the `milliseconds` suffix in the spinbox instead of the label
            textFromValue: function (value) {
                return value + " " + i18n("milliseconds");
            }

            // Parse the integer value from the spinbox text, ignoring the suffix
            valueFromText: function (text) {
                let number = parseInt(text);
                if (isNaN(number)) {
                    return kcm.diacriticsHoldThresholdMs; // Fallback to current value if parsing fails
                }
                return number;
            }

            onValueChanged: {
                kcm.diacriticsHoldThresholdMs = value;
                value = Qt.binding(() => kcm.diacriticsHoldThresholdMs);
            }
        }

        QQC2.SpinBox {
            id: keyboardHeightSpinBox
            Kirigami.FormData.label: i18n("Keyboard height:")
            from: 20
            to: 80
            stepSize: 2
            value: kcm.keyboardHeightPercent

            textFromValue: function (value) {
                return value + "%";
            }
            valueFromText: function (text) {
                let number = parseInt(text);
                return isNaN(number) ? kcm.keyboardHeightPercent : number;
            }

            onValueChanged: {
                kcm.keyboardHeightPercent = value;
                value = Qt.binding(() => kcm.keyboardHeightPercent);
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Keyboard font:")

            QQC2.Button {
                id: keyboardFontButton
                Layout.fillWidth: true
                text: kcm.keyboardFontFamily.length > 0 ? kcm.keyboardFontFamily : i18n("Default")
                onClicked: fontDialog.open()
            }

            QQC2.Button {
                text: i18n("Reset")
                enabled: kcm.keyboardFontFamily.length > 0
                onClicked: kcm.keyboardFontFamily = ""
            }
        }
    }

    FontDialog {
        id: fontDialog
        title: i18n("Select Keyboard Font")
        onAccepted: kcm.keyboardFontFamily = fontDialog.selectedFont.family
    }
}
