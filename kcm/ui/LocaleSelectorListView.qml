/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.kirigami as Kirigami
import org.kde.kquickcontrols as KQuickControls

import "localeutils.js" as LocaleUtils

/**
 * The layouts the keyboard switches between, shown the way the system keyboard
 * KCM shows physical layouts: flag, name, locale code, the "open by default"
 * mark, removal and a drag handle that reorders the switch ring. Layouts are
 * added through LocaleChooserDialog, not by ticking every known language.
 */
ListView {
    id: root

    //! Every locale the keyboard ships, needed by the "Add Layout" dialog.
    property var availableLocales: []

    model: kcm.enabledLocales

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

    Component.onCompleted: availableLocales = VirtualKeyboardSettings.availableLocales

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            root.availableLocales = VirtualKeyboardSettings.availableLocales;
        }
    }

    headerPositioning: ListView.OverlayHeader
    header: QQC2.ToolBar {
        width: parent.width
        z: 999 // On top of content
        position: QQC2.ToolBar.Header

        topPadding: Kirigami.Units.largeSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing

        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.largeSpacing

            SettingsRow {
                Layout.fillWidth: true
                label: i18n("Try it:")
                description: i18n("Start typing to check the settings")
                controlFillWidth: true

                QQC2.TextField {
                    Layout.fillWidth: true
                    placeholderText: i18n("Type here to see the keyboard")
                }
            }

            SettingsRow {
                Layout.fillWidth: true
                label: i18n("Shortcut for showing the keyboard:")

                KQuickControls.KeySequenceItem {
                    id: shortcutItem
                    keySequence: kcm.shortcut

                    onKeySequenceModified: {
                        kcm.setShortcut(keySequence);
                        keySequence = Qt.binding(() => kcm.shortcut);
                    }
                }

                QQC2.Button {
                    text: i18n("Default")
                    onClicked: kcm.resetShortcut()
                }
            }

            RowLayout {
                QQC2.Button {
                    text: i18n("Add…")
                    icon.name: "list-add"
                    onClicked: layoutDialog.open()
                }

                Item {
                    Layout.fillWidth: true
                }
            }

            Kirigami.InlineMessage {
                Layout.fillWidth: true
                text: i18n("No languages selected. The default keyboard layout for the system will be used.")
                type: Kirigami.MessageType.Information
                visible: kcm.enabledLocales.length === 0
            }
        }
    }

    delegate: Item {
        id: itemDelegate

        width: ListView.view.width
        implicitHeight: layoutDelegate.implicitHeight

        readonly property var view: ListView.view

        required property string modelData
        required property int index

        QQC2.ItemDelegate {
            id: layoutDelegate
            width: itemDelegate.width

            // There's no need for a list item to ever be selected
            down: false
            highlighted: false

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.ListItemDragHandle {
                    listItem: layoutDelegate
                    listView: itemDelegate.view
                    onMoveRequested: (oldIndex, newIndex) => kcm.moveLocale(itemDelegate.modelData, newIndex)
                    visible: itemDelegate.view.count > 1
                }

                QQC2.Label {
                    text: LocaleUtils.flagForLocale(itemDelegate.modelData)
                    font.pixelSize: Kirigami.Units.iconSizes.smallMedium
                    visible: text.length > 0
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: Qt.locale(itemDelegate.modelData).nativeLanguageName
                        elide: Text.ElideRight
                    }

                    QQC2.Label {
                        text: itemDelegate.modelData
                        font: Kirigami.Theme.smallFont
                        opacity: 0.6
                    }
                }

                // Marks the layout the keyboard opens with.
                QQC2.ToolButton {
                    Layout.alignment: Qt.AlignVCenter
                    icon.name: kcm.defaultLocale === itemDelegate.modelData ? "starred" : "non-starred"
                    checkable: true
                    checked: kcm.defaultLocale === itemDelegate.modelData
                    onClicked: kcm.setDefaultLocale(checked ? itemDelegate.modelData : "")
                    Accessible.name: i18n("Open this layout by default")
                    QQC2.ToolTip.text: i18n("Open by default")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }

                QQC2.ToolButton {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    icon.name: "edit-delete"
                    onClicked: kcm.disableLocale(itemDelegate.modelData)
                    Accessible.name: i18n("Remove this layout")
                    QQC2.ToolTip.text: i18n("Remove")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                }
            }
        }
    }

    LocaleChooserDialog {
        id: layoutDialog
        availableLocales: root.availableLocales
    }
}
