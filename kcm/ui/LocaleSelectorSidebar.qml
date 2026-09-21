/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.bigscreen as Bigscreen

import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import "localeutils.js" as LocaleUtils

/**
 * Bigscreen version of LocaleSelectorListView: the layouts the keyboard
 * switches between, with adding done through LocaleChooserDialog.
 */
Bigscreen.SidebarOverlay {
    id: root

    //! Every locale the keyboard ships, needed by the "Add Layout" dialog.
    property var availableLocales: []

    openFocusItem: addButton

    header: Bigscreen.SidebarOverlayHeader {
        title: i18n("Languages")
    }

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

    Component.onCompleted: availableLocales = VirtualKeyboardSettings.availableLocales

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            root.availableLocales = VirtualKeyboardSettings.availableLocales;
        }
    }

    content: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        QQC2.Button {
            id: addButton
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
            text: i18n("Add…")
            icon.name: "list-add"
            onClicked: layoutDialog.open()
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
            text: i18n("No languages selected. The default keyboard layout for the system will be used.")
            type: Kirigami.MessageType.Information
            visible: kcm.enabledLocales.length === 0
        }

        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: langs
                clip: true
                model: kcm.enabledLocales

                delegate: QQC2.ItemDelegate {
                    id: localeDelegate

                    width: langs.width
                    down: false
                    highlighted: false

                    required property string modelData
                    required property int index

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            text: LocaleUtils.flagForLocale(localeDelegate.modelData)
                            font.pixelSize: Kirigami.Units.iconSizes.smallMedium
                            visible: text.length > 0
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 0

                            QQC2.Label {
                                Layout.fillWidth: true
                                text: Qt.locale(localeDelegate.modelData).nativeLanguageName
                                elide: Text.ElideRight
                            }

                            QQC2.Label {
                                text: localeDelegate.modelData
                                font: Kirigami.Theme.smallFont
                                opacity: 0.6
                            }
                        }

                        // See LocaleSelectorListView.qml.
                        QQC2.ToolButton {
                            Layout.alignment: Qt.AlignVCenter
                            icon.name: kcm.defaultLocale === localeDelegate.modelData ? "starred" : "non-starred"
                            checkable: true
                            checked: kcm.defaultLocale === localeDelegate.modelData
                            onClicked: kcm.setDefaultLocale(checked ? localeDelegate.modelData : "")
                            Accessible.name: i18n("Open this layout by default")
                            QQC2.ToolTip.text: i18n("Open by default")
                            QQC2.ToolTip.visible: hovered
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                        }

                        QQC2.ToolButton {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.rightMargin: Kirigami.Units.smallSpacing
                            icon.name: "edit-delete"
                            onClicked: kcm.disableLocale(localeDelegate.modelData)
                            Accessible.name: i18n("Remove this layout")
                            QQC2.ToolTip.text: i18n("Remove")
                            QQC2.ToolTip.visible: hovered
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                        }
                    }
                }
            }
        }
    }

    LocaleChooserDialog {
        id: layoutDialog
        availableLocales: root.availableLocales
    }
}
