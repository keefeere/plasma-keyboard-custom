/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.bigscreen as Bigscreen

import org.kde.kitemmodels
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

Bigscreen.SidebarOverlay {
    id: root

    property alias model: langs.model

    openFocusItem: langs
    header: Bigscreen.SidebarOverlayHeader {
        title: i18n("Languages")
    }

    function updateModel() {
        let list = [];
        for (let locale of sourceModel) {
            const localeText = Qt.locale(locale).nativeLanguageName;
            if (searchText.length === 0 || localeText.toLowerCase().indexOf(searchText.toLowerCase()) !== -1) {
                list.push(locale);
            }
        }
        model = list;
    }

    onModelChanged: updateModel()

    Connections {
        target: VirtualKeyboardSettings

        function onAvailableLocalesChanged() {
            langs.model = VirtualKeyboardSettings.availableLocales;
        }
    }

    // HACK: needed to populate VirtualKeyboardSettings.availableLocales
    InputPanel {}

    content: QQC2.ScrollView {
        ListView {
            id: langs
            Layout.fillWidth: true
            implicitHeight: contentHeight
            clip: true

            delegate: RowLayout {
                width: langs.width
                spacing: 0

                Bigscreen.SwitchDelegate {
                    Layout.fillWidth: true
                    text: Qt.locale(modelData).nativeLanguageName
                    onClicked: checked = !checked
                    Keys.onReturnPressed: checked = !checked
                    checked: kcm.enabledLocales.includes(modelData)
                    onCheckedChanged: {
                        if (checked) {
                            kcm.enableLocale(modelData);
                        } else {
                            kcm.disableLocale(modelData);
                        }
                    }
                }

                // See LocaleSelectorListView.qml.
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
    }
}
