/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

KCM.SimpleKCM {
    id: root

    leftPadding: 0
    rightPadding: 0
    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit

    ColumnLayout {
        spacing: 0
        width: parent.width

        FormCard.FormCard {
            FormCard.FormTextFieldDelegate {
                label: i18n("Test the virtual keyboard…")
            }
        }

        FormCard.FormCard {
            Layout.topMargin: Kirigami.Units.largeSpacing

            FormCard.FormButtonDelegate {
                id: languageList
                text: i18n("Languages")
                icon.name: 'languages'
                onClicked: kcm.push(localePage)

                Kirigami.ScrollablePage {
                    id: localePage
                    title: i18n("Keyboard Languages")

                    LocaleSelectorListView {}
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group", "Opening the keyboard")
        }

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: showOnLongTap
                text: i18n("Open on long press")
                description: i18n("Hold a finger on a text field to open the keyboard")

                checked: kcm.showOnLongTap
                onCheckedChanged: {
                    kcm.showOnLongTap = checked;
                    checked = Qt.binding(() => kcm.showOnLongTap)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSpinBoxDelegate {
                id: showOnLongTapThreshold
                label: i18n("Long press delay")
                description: i18n("Time to hold a finger on the screen")
                from: 100
                to: 5000
                stepSize: 100
                enabled: showOnLongTap.checked
                value: kcm.showOnLongTapThresholdMs
                onValueChanged: kcm.showOnLongTapThresholdMs = value

                textFromValue: function (value) {
                    return i18nc("duration in milliseconds", "%1 ms", value);
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: showOnMouseFocus
                text: i18n("Open when focused with a mouse")
                description: i18n("Otherwise it only opens on touch input or via the shortcut")

                checked: kcm.showOnMouseFocus
                onCheckedChanged: {
                    kcm.showOnMouseFocus = checked;
                    checked = Qt.binding(() => kcm.showOnMouseFocus)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: hidePanelWhenKeyboardVisible
                text: i18n("Hide the panel while the keyboard is open")
                description: i18n("The keyboard reaches the bottom of the screen")

                checked: kcm.hidePanelWhenKeyboardVisible
                onCheckedChanged: {
                    kcm.hidePanelWhenKeyboardVisible = checked;
                    checked = Qt.binding(() => kcm.hidePanelWhenKeyboardVisible)
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group", "Appearance")
        }

        FormCard.FormCard {
            FormCard.FormSpinBoxDelegate {
                label: i18n("Keyboard height (%)")
                description: i18n("Percentage of the screen height")
                from: 20
                to: 80
                stepSize: 2
                value: kcm.keyboardHeightPercent
                onValueChanged: kcm.keyboardHeightPercent = value
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormComboBoxDelegate {
                id: keyboardFontComboBox
                text: i18n("Keyboard font")

                model: [i18n("Default")].concat(Qt.fontFamilies())
                currentIndex: Math.max(0, model.indexOf(kcm.keyboardFontFamily))

                onActivated: (index) => {
                    kcm.keyboardFontFamily = index === 0 ? "" : model[index];
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: showFunctionKeyRow
                text: i18n("Function keys")
                description: i18n("Show an F1–F12 row above the keyboard")

                checked: kcm.showFunctionKeyRow
                onCheckedChanged: {
                    kcm.showFunctionKeyRow = checked;
                    checked = Qt.binding(() => kcm.showFunctionKeyRow)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: clipboardEnabled
                text: i18n("Clipboard")
                description: i18n("Show recent clipboard entries above the keyboard")

                checked: kcm.clipboardEnabled
                onCheckedChanged: {
                    kcm.clipboardEnabled = checked;
                    checked = Qt.binding(() => kcm.clipboardEnabled)
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group", "Typing")
        }

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: autoCapitalizationEnabled
                text: i18n("Automatic capitalization")
                description: i18n("Capitalize the first letter of a sentence")

                checked: kcm.autoCapitalizationEnabled
                onCheckedChanged: {
                    kcm.autoCapitalizationEnabled = checked;
                    checked = Qt.binding(() => kcm.autoCapitalizationEnabled)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: diacriticsCheckbox
                text: i18n("Alternate characters")
                description: i18n("Show a popup when holding a key")

                checked: kcm.diacriticsPopupEnabled
                onCheckedChanged: {
                    kcm.diacriticsPopupEnabled = checked;
                    checked = Qt.binding(() => kcm.diacriticsPopupEnabled)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSpinBoxDelegate {
                label: i18n("Hold delay")
                description: i18n("Time to hold a key before the popup appears")
                from: 100
                to: 1500
                stepSize: 50
                enabled: diacriticsCheckbox.checked
                value: kcm.diacriticsHoldThresholdMs
                onValueChanged: kcm.diacriticsHoldThresholdMs = value

                textFromValue: function (value) {
                    return i18nc("duration in milliseconds", "%1 ms", value);
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title:group", "Feedback")
        }

        FormCard.FormCard {
            FormCard.FormSwitchDelegate {
                id: soundsEnabled
                text: i18n("Sound")
                description: i18n("Whether to emit a sound on key press")

                checked: kcm.soundEnabled
                onCheckedChanged: {
                    kcm.soundEnabled = checked;
                    checked = Qt.binding(() => kcm.soundEnabled)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: vibrationEnabled
                text: i18n("Vibration")
                description: i18n("Whether to vibrate on key press")

                checked: kcm.vibrationEnabled
                onCheckedChanged: {
                    kcm.vibrationEnabled = checked;
                    checked = Qt.binding(() => kcm.vibrationEnabled)
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSpinBoxDelegate {
                label: i18n("Vibration strength")
                description: i18n("Percentage of the maximum vibration")
                from: 0
                to: 100
                stepSize: 5
                enabled: vibrationEnabled.checked
                value: kcm.vibrationStrength
                onValueChanged: kcm.vibrationStrength = value

                textFromValue: function (value) {
                    return i18nc("vibration strength in percent", "%1%", value);
                }
            }

            FormCard.FormDelegateSeparator {}

            FormCard.FormSwitchDelegate {
                id: keyboardNavigationEnabled
                text: i18n("Keyboard navigation")
                description: i18n("Whether to use the arrow keys to navigate the keyboard")

                checked: kcm.keyboardNavigationEnabled
                onCheckedChanged: {
                    kcm.keyboardNavigationEnabled = checked;
                    checked = Qt.binding(() => kcm.keyboardNavigationEnabled)
                }
            }
        }
    }
}
