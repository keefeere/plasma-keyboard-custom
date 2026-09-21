/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import "localeutils.js" as LocaleUtils

/**
 * Picker for the keyboard layouts, modelled after the "Add Layout" dialog of
 * the system keyboard KCM. Qt VirtualKeyboard ships a single layout per locale,
 * so unlike the system dialog there is no variant column: one list of locales.
 */
Kirigami.Dialog {
    id: dialog

    //! Every locale the keyboard ships, in the order Qt reports them.
    property var availableLocales: []
    property string searchText: ""

    function updateModel() {
        const search = searchText.trim().toLowerCase();
        let list = [];

        for (const locale of availableLocales) {
            if (kcm.enabledLocales.includes(locale)) {
                continue;
            }

            const name = Qt.locale(locale).nativeLanguageName;
            if (search.length === 0 || name.toLowerCase().includes(search) || locale.toLowerCase().includes(search)) {
                list.push(locale);
            }
        }

        localeList.model = list;
        localeList.currentIndex = list.length > 0 ? 0 : -1;
    }

    title: i18n("Add Layout")
    implicitWidth: Kirigami.Units.gridUnit * 26
    implicitHeight: Kirigami.Units.gridUnit * 24
    padding: Kirigami.Units.largeSpacing

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    onAvailableLocalesChanged: updateModel()

    onOpened: {
        searchField.text = "";
        updateModel();
        searchField.forceActiveFocus();
    }

    onAccepted: {
        if (localeList.currentIndex >= 0) {
            kcm.enableLocale(localeList.model[localeList.currentIndex]);
        }
    }

    Component.onCompleted: {
        const okButton = standardButton(Kirigami.Dialog.Ok);
        okButton.enabled = Qt.binding(() => localeList.currentIndex >= 0);
    }

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.SearchField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: i18n("Filter languages…")
            Accessible.name: i18n("Filter languages")

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
                id: localeList
                clip: true
                model: []
                currentIndex: -1
                keyNavigationEnabled: true
                activeFocusOnTab: true

                delegate: QQC2.ItemDelegate {
                    id: localeDelegate

                    width: ListView.view.width
                    highlighted: ListView.isCurrentItem

                    required property string modelData
                    required property int index

                    onClicked: localeList.currentIndex = index

                    Accessible.name: Qt.locale(localeDelegate.modelData).nativeLanguageName

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            text: LocaleUtils.flagForLocale(localeDelegate.modelData)
                            font.pixelSize: Kirigami.Units.iconSizes.smallMedium
                            visible: text.length > 0
                        }

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
                }
            }
        }
    }
}
