/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

/**
 * A single setting: its name on the left, the control on the right and an
 * optional explanation under the row.
 *
 * The control sits in a column of a fixed share of the row width, so sliders,
 * combo boxes, spin boxes and fields are all the same length. A trailing item
 * (for example the percent value of a slider) goes into a narrow column after
 * that, which keeps every control aligned.
 *
 * Unlike Kirigami.FormLayout, which right-aligns the labels of the left column
 * (so they end up in the middle of a wide window), the name stays on the left
 * and rows are separated by a comfortable gap.
 */
ColumnLayout {
    id: root

    //! Name of the setting, left-aligned.
    property string label: ""
    //! Optional explanation shown under the row in a faint colour.
    property string description: ""
    //! Stretch the control across the control column (sliders, combo boxes,
    //! spin boxes, text fields). Without it the control keeps its natural size
    //! and stays at the right edge (switches, buttons).
    property bool controlFillWidth: false
    //! Share of the row width taken by the control column.
    property real controlWidthRatio: 0.55
    //! Width reserved after the control column for the trailing item.
    property real trailingWidth: Kirigami.Units.gridUnit * 3
    //! Item placed in the trailing column, for example a value label.
    property Item trailing: null

    default property alias content: controlRow.data

    spacing: Kirigami.Units.smallSpacing

    RowLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.largeSpacing

        QQC2.Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            text: root.label
            wrapMode: Text.Wrap
            maximumLineCount: 2
            elide: Text.ElideRight
        }

        Item {
            id: controlHolder
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: Math.round(root.width * root.controlWidthRatio)
            implicitHeight: controlRow.implicitHeight

            RowLayout {
                id: controlRow
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: root.controlFillWidth ? parent.width : implicitWidth
                spacing: Kirigami.Units.smallSpacing
            }
        }

        Item {
            id: trailingHolder
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: root.trailingWidth
            implicitHeight: trailingRow.implicitHeight

            RowLayout {
                id: trailingRow
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                spacing: 0

                LayoutItemProxy {
                    target: root.trailing
                    visible: target !== null
                }
            }
        }
    }

    QQC2.Label {
        Layout.fillWidth: true
        visible: root.description.length > 0
        text: root.description
        color: Kirigami.Theme.disabledTextColor
        font: Kirigami.Theme.smallFont
        wrapMode: Text.Wrap
    }
}
