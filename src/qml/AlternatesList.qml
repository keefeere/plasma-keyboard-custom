/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

pragma ComponentBehavior: Bound

import QtQuick

/*!
The alternate characters of the key the gamepad highlight is on.

The list is drawn exactly the way Qt Virtual Keyboard draws its own
alternate-keys popup: the delegate, the highlight and the background are the
components of the keyboard style, so it looks the same and follows the theme
the user chose. It deliberately lives inside the keyboard window — a window of
its own takes the input focus away from the field being typed into, and the
character picked from the list would then have nowhere to be inserted.

The selection is driven from the gamepad, which sends no key events: the
controller owns the selected index and it is mirrored here.
*/
Item {
    id: root

    /** The keyboard style, for the look of the list. */
    required property var style

    /** The characters to offer, in the order they should appear. */
    property var options: []

    /** Index selected from the outside (the gamepad), -1 for none. */
    property int externalSelectedIndex: -1

    /** Horizontal centre of the key the list belongs to, in the coordinates of
        this item's parent. */
    property real anchorX: 0

    /** Vertical position of the top of that key, in the coordinates of this
        item's parent. */
    property real anchorY: 0

    /** Index the user picked, by tap or by the gamepad. */
    signal characterSelected(int index)

    readonly property alias count: listModel.count

    // Position of the list, fixed while it is open. Only the highlight moves
    // when the gamepad walks the entries, exactly like Qt Virtual Keyboard's
    // own popup: it computes its origin once, when it opens, and then leaves
    // the list where it is.
    property real listX: 0
    property real listY: 0

    // The style is what defines the look; its own defaults are walked back to
    // so a style that leaves them unset still gives a usable list.
    readonly property real itemWidth: style && style.alternateKeysListItemWidth > 0 ? style.alternateKeysListItemWidth : 1
    readonly property real itemHeight: style && style.alternateKeysListItemHeight > 0 ? style.alternateKeysListItemHeight : 1

    z: 1
    anchors.fill: parent

    ListModel {
        id: listModel
    }

    //! Rebuild the list from options; the entries carry both what is shown and
    //! what is inserted, like Qt Virtual Keyboard's own popup does.
    function rebuild() {
        listModel.clear();
        for (let i = 0; i < options.length; i++) {
            listModel.append({
                "text": options[i],
                "data": options[i]
            });
        }
        syncSelection();
        updatePosition();
    }

    //! Mirror the index the controller selected.
    function syncSelection() {
        if (listModel.count === 0) {
            return;
        }
        listView.currentIndex = externalSelectedIndex >= 0 && externalSelectedIndex < listModel.count ? externalSelectedIndex : 0;
    }

    //! Put the list over the key it belongs to: the selected entry is centred
    //! on it, as Qt Virtual Keyboard centres its own popup. Called when the
    //! list opens or the key under the highlight changes — never while the
    //! gamepad walks the entries, so the list stays put.
    function updatePosition() {
        if (listView.width <= 0 || listModel.count === 0) {
            return;
        }

        const centred = (Math.max(0, listView.currentIndex) + 0.5) * itemWidth;
        const left = style ? style.alternateKeysListLeftMargin : 0;
        const right = style ? style.alternateKeysListRightMargin : 0;
        listX = Math.max(left, Math.min(width - listView.width - right, anchorX - centred));

        const bottomMargin = style ? style.alternateKeysListBottomMargin : 0;
        listY = Math.max(0, anchorY - listView.height - bottomMargin);
    }

    onOptionsChanged: rebuild()
    onExternalSelectedIndexChanged: syncSelection()
    onAnchorXChanged: updatePosition()
    onAnchorYChanged: updatePosition()
    onWidthChanged: updatePosition()
    onHeightChanged: updatePosition()

    ListView {
        id: listView

        model: listModel
        delegate: root.style ? root.style.alternateKeysListDelegate : null
        highlight: root.style && root.style.alternateKeysListHighlight ? root.style.alternateKeysListHighlight : defaultHighlight
        highlightMoveDuration: 0
        highlightResizeDuration: 0
        orientation: ListView.Horizontal
        spacing: 0
        // The gamepad drives the list: it must not take the focus or react to
        // keys on its own, or it would fight the keyboard navigation.
        interactive: false
        focus: false

        width: root.itemWidth * listModel.count
        height: root.itemHeight

        // Fixed while the list is open (see updatePosition); the highlight is
        // what moves when the gamepad walks the entries.
        x: root.listX
        y: root.listY

        // The width follows the number of entries, so the position has to be
        // worked out again once the list has grown to its real size.
        onWidthChanged: root.updatePosition()
        onHeightChanged: root.updatePosition()

        Component {
            id: defaultHighlight
            Item {}
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse => {
                const index = Math.floor(mouse.x / root.itemWidth);
                if (index >= 0 && index < listModel.count) {
                    root.characterSelected(index);
                }
            }
        }
    }

    // The popup background of the style, drawn behind the list.
    Loader {
        id: backgroundLoader

        anchors.fill: listView
        sourceComponent: root.style ? root.style.alternateKeysListBackground : null
        z: -1
    }
}
