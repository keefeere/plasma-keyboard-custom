/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami

ListView {
    id: root

    property var sourceModel: []
    property string searchText: ''

    model: sourceModel

    function updateModel() {
        let list = [];
        for (let locale of sourceModel) {
            const localeText = Qt.locale(locale).nativeLanguageName;
            if (searchText.length === 0 || localeText.toLowerCase().indexOf(searchText.toLowerCase()) !== -1) {
                list.push(locale)
            }
        }
        model = list;
    }
    onSearchTextChanged: updateModel()
    onSourceModelChanged: updateModel()

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            root.sourceModel = VirtualKeyboardSettings.availableLocales;
        }
    }

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

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
            spacing: 0

            Kirigami.SearchField {
                id: searchField
                placeholderText: i18n("Filter languages…")
                Accessible.name: i18n("Filter languages")

                Layout.fillWidth: true

                onTextChanged: {
                    root.searchText = text;
                    searchField.forceActiveFocus();
                }
            }

            Kirigami.InlineMessage {
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.fillWidth: true
                text: i18n("No languages selected. The default keyboard layout for the system will be used.")
                type: Kirigami.MessageType.Information
                visible: kcm.enabledLocales.length === 0
            }
        }
    }

    delegate: RowLayout {
        width: root.width
        spacing: 0

        QQC2.CheckDelegate {
            Layout.fillWidth: true
            text: Qt.locale(modelData).nativeLanguageName
            checked: kcm.enabledLocales.includes(modelData)
            onCheckedChanged: {
                if (checked) {
                    kcm.enableLocale(modelData);
                } else {
                    kcm.disableLocale(modelData);
                }
            }
        }

        // Marks the layout the keyboard opens with. Only an enabled locale can
        // be chosen, and picking one makes it win over the system locale.
        QQC2.ToolButton {
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: Kirigami.Units.smallSpacing
            visible: kcm.enabledLocales.includes(modelData)
            icon.name: kcm.defaultLocale === modelData ? "starred" : "non-starred"
            checkable: true
            checked: kcm.defaultLocale === modelData
            onClicked: kcm.setDefaultLocale(checked ? modelData : "")
            Accessible.name: i18n("Open this layout by default")
            QQC2.ToolTip.text: i18n("Open by default")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
        }
    }
}
