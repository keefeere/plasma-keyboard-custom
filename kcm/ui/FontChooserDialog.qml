/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

/**
 * Font family picker for the keyboard: a search field and a list where every
 * entry is drawn in its own font, with a sample next to it.
 *
 * The system FontDialog of QtQuick.Dialogs does not show up in the KCM window,
 * so the picker lives in the KCM itself, like the layout chooser.
 */
Kirigami.Dialog {
    id: dialog

    //! Every font family known to the system.
    property var families: Qt.fontFamilies()
    //! Family the dialog was opened with, and the one the user picked.
    property string selectedFamily: ""
    property string searchText: ""

    function updateModel() {
        const search = searchText.trim().toLowerCase();
        let list = [];

        for (const family of families) {
            if (search.length === 0 || family.toLowerCase().includes(search)) {
                list.push(family);
            }
        }

        fontList.model = list;

        const current = list.indexOf(selectedFamily);
        fontList.currentIndex = current >= 0 ? current : (list.length > 0 ? 0 : -1);
    }

    title: i18n("Choose keyboard font")
    implicitWidth: Kirigami.Units.gridUnit * 30
    implicitHeight: Kirigami.Units.gridUnit * 26
    padding: Kirigami.Units.largeSpacing

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onOpened: {
        searchField.text = "";
        updateModel();
        searchField.forceActiveFocus();
    }

    onAccepted: selectedFamily = fontList.model[fontList.currentIndex]

    Component.onCompleted: {
        const okButton = standardButton(Kirigami.Dialog.Ok);
        okButton.enabled = Qt.binding(() => fontList.currentIndex >= 0);
    }

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.SearchField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: i18n("Filter fonts…")
            Accessible.name: i18n("Filter fonts")

            onTextChanged: {
                dialog.searchText = text;
                dialog.updateModel();
            }
        }

        QQC2.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Kirigami.StyleHints.showFramedBackground: true

            ListView {
                id: fontList
                clip: true
                model: []
                currentIndex: -1
                keyNavigationEnabled: true
                activeFocusOnTab: true

                delegate: QQC2.ItemDelegate {
                    id: fontDelegate

                    width: ListView.view.width
                    highlighted: ListView.isCurrentItem

                    required property string modelData
                    required property int index

                    onClicked: fontList.currentIndex = index

                    Accessible.name: fontDelegate.modelData

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.largeSpacing

                        QQC2.Label {
                            Layout.fillWidth: true
                            text: fontDelegate.modelData
                            font.family: fontDelegate.modelData
                            elide: Text.ElideRight
                        }

                        QQC2.Label {
                            text: "Abc 123"
                            font.family: fontDelegate.modelData
                            opacity: 0.7
                        }
                    }
                }
            }
        }
    }
}
