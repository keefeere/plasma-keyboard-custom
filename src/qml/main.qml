/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

import org.kde.plasma.keyboard.custom
import org.kde.plasma.keyboard.custom.lib as PlasmaKeyboard

import org.kde.kirigami as Kirigami

InputPanelWindow {
    id: root
    height: Screen.height
    width: Screen.width
    color: 'transparent'

    onVisibleChanged: {
        if (!visible) {
            // Reset keyboard navigation when hidden
            // Note: keyboard property is internal Qt API
            if (inputPanel.keyboard.navigationModeActive) {
                inputPanel.keyboard.navigationModeActive = false;
            }

            // Do not keep a focus in the rows above the keyboard either.
            extraRowFocus = 0;
            extraColumn = 0;

            // Close language dialog
            languageDialog.close();
        }
    }

    // Gamepad: which of the rows above the keyboard has the focus
    // (0 = the keyboard itself, 1 = the clipboard row, 2 = the F1-F12 row) and
    // which item of it is selected. Those rows are plain items, so the
    // navigation of Qt Virtual Keyboard cannot reach them on its own.
    property int extraRowFocus: 0
    property int extraColumn: 0

    //! Rows above the keyboard, listed from the keyboard upwards. Rebuilt
    //! whenever a row is shown or hidden, so the whole navigation follows this
    //! one list and disabled or empty rows simply drop out of it.
    readonly property var navRows: {
        const rows = [];
        if (functionKeyRow.visible) {
            rows.push("fkeys");
        }
        if (clipboardRow.visible) {
            rows.push("clipboard");
        }
        return rows;
    }

    //! Number of selectable items in the given row.
    function extraRowItemCount(zone) {
        if (zone <= 0 || zone > navRows.length) {
            return 0;
        }
        // The clipboard row also holds the key that clears the history.
        return navRows[zone - 1] === "clipboard" ? thing.clipboardHistory.count + 1 : 12;
    }

    //! Zone number of the named row (0 when that row is not shown).
    function zoneOf(name) {
        return navRows.indexOf(name) + 1;
    }

    //! Row above the given one (0 = none above the keyboard).
    function rowAbove(zone) {
        const index = zone === 0 ? 0 : zone;
        return index < navRows.length ? index + 1 : 0;
    }

    //! Row below the given one (0 = the keyboard itself).
    function rowBelow(zone) {
        return zone > 1 ? zone - 1 : 0;
    }

    //! Row the focus wraps to when it leaves the keyboard downwards.
    function topExtraRow() {
        return navRows.length;
    }

    //! Item of the given row that lies under the given horizontal fraction.
    function columnForZone(zone, ratio) {
        const count = extraRowItemCount(zone);
        if (count <= 0) {
            return 0;
        }
        return Math.max(0, Math.min(count - 1, Math.floor(ratio * count)));
    }

    //! Moves the keyboard focus sideways to the column under the given fraction.
    function focusKeyboardColumn(ratio) {

        const keyboard = inputPanel.keyboard;
        const highlight = keyboard.navigationHighlight;
        const targetX = ratio * keyboard.width;
        for (let i = 0; i < 12; ++i) {
            const item = highlight ? highlight.highlightItem : null;
            if (!item || item === keyboard) {
                break;
            }
            const centreX = keyboard.mapFromItem(item, item.width / 2, 0).x;
            if (Math.abs(centreX - targetX) < item.width / 2) {
                break;
            }
            const direction = centreX < targetX ? Qt.Key_Right : Qt.Key_Left;
            inputPanel.InputContext.priv.navigationKeyPressed(direction, false);
            inputPanel.InputContext.priv.navigationKeyReleased(direction, false);
        }
    }

    //! Moves the keyboard focus to the first or last row of keys, keeping the
    //! horizontal position under the given fraction.
    function focusKeyboardEdge(ratio, bottom) {

        const keyboard = inputPanel.keyboard;
        const highlight = keyboard.navigationHighlight;
        const rowHeight = keyboard.style ? keyboard.style.targetKeyboardHeight / 5 : keyboard.height / 5;

        // Nothing highlighted yet: one press starts the navigation and moves in
        // the wanted direction.
        if (!highlight || highlight.highlightItem === keyboard) {
            const key = bottom ? Qt.Key_Down : Qt.Key_Up;
            inputPanel.InputContext.priv.navigationKeyPressed(key, false);
            inputPanel.InputContext.priv.navigationKeyReleased(key, false);
        }

        for (let i = 0; i < 6; ++i) {
            const item = highlight ? highlight.highlightItem : null;
            if (!item || item === keyboard) {
                break;
            }
            const centreY = keyboard.mapFromItem(item, 0, item.height / 2).y;
            const atEdge = bottom ? centreY > keyboard.height - rowHeight : centreY < rowHeight;
            if (atEdge) {
                break;
            }
            const key = bottom ? Qt.Key_Down : Qt.Key_Up;
            inputPanel.InputContext.priv.navigationKeyPressed(key, false);
            inputPanel.InputContext.priv.navigationKeyReleased(key, false);
        }

        focusKeyboardColumn(ratio);
    }

    //! Activates the selected item of the focused row.
    function activateExtraRowItem() {
        if (extraRowFocus === zoneOf("clipboard")) {
            if (extraColumn < thing.clipboardHistory.count) {
                thing.commitText(thing.clipboardHistory.textAt(extraColumn));
            } else {
                thing.clipboardHistory.clear();
            }
            return;
        }
        if (extraRowFocus === zoneOf("fkeys")) {
            thing.sendKeyEvent(Qt.Key_F1 + extraColumn, "");
        }
    }

    // While the focus is in the row right above the keyboard, keep the keyboard
    // cursor under the selected item, so leaving downwards lands on the key
    // that is below it.
    onExtraColumnChanged: {
        if (extraRowFocus === 1) {
            focusKeyboardEdge((extraColumn + 0.5) / extraRowItemCount(1), false);
        }
    }

    InputListenerItem {
        id: thing
        focus: true
        engine: inputPanel.InputContext.inputEngine

        keyboardNavigationActive: inputPanel.keyboard.navigationModeActive

        onKeyNavigationPressed: (key) => {
            // HACK: invoke the Qt VirtualKeyboard keyboard navigation feature ourselves
            // See https://github.com/qt/qtvirtualkeyboard/blob/6d810ac41df96f1ad984f56e17f16860bec2abbf/src/virtualkeyboard/qvirtualkeyboardinputcontext_p.h#L110
            inputPanel.InputContext.priv.navigationKeyPressed(key, false);
        }
        onKeyNavigationReleased: (key) => {
            // HACK: invoke the Qt VirtualKeyboard keyboard navigation feature ourselves
            inputPanel.InputContext.priv.navigationKeyReleased(key, false);
        }
    }

    // Gamepad support (via InputPlumber's dbus target on the system bus).
    GamepadHandler {
        id: gamepad
        onNavigate: (key) => {
            if (root.extraRowFocus !== 0) {
                const count = root.extraRowItemCount(root.extraRowFocus);
                if (key === Qt.Key_Left) {
                    root.extraColumn = (root.extraColumn + count - 1) % count;
                    return;
                }
                if (key === Qt.Key_Right) {
                    root.extraColumn = (root.extraColumn + 1) % count;
                    return;
                }

                // Up and down move along the rows above the keyboard. Going
                // down past the lowest row returns to the keyboard, going up
                // past the highest one stays there: the row above is the top of
                // the list, so a held direction cannot fall out of it.
                if (key === Qt.Key_Up) {
                    const above = root.rowAbove(root.extraRowFocus);
                    const ratio = (root.extraColumn + 0.5) / count;
                    if (above !== 0) {
                        root.extraRowFocus = above;
                        root.extraColumn = root.columnForZone(above, ratio);
                    } else {
                        // Top of the circle: continue on the bottom row.
                        root.extraRowFocus = 0;
                        root.extraColumn = 0;
                        root.focusKeyboardEdge(ratio, true);
                    }
                    return;
                }

                const below = root.rowBelow(root.extraRowFocus);
                const ratio = (root.extraColumn + 0.5) / count;
                if (below !== 0) {
                    root.extraRowFocus = below;
                    root.extraColumn = root.columnForZone(below, ratio);
                    // The row right above the keyboard: put its focus on the
                    // first row of keys now, while the highlight is hidden, so
                    // leaving downwards needs no movement at all.
                    if (below === 1) {
                        root.focusKeyboardEdge(ratio, false);
                    }
                } else {
                    // Back to the keyboard, on the key below the one that was
                    // selected in the row.
                    root.extraRowFocus = 0;
                    root.extraColumn = 0;
                    root.focusKeyboardEdge(ratio, false);
                }
                return;
            }

            // Qt Virtual Keyboard wraps sideways across rows: on the first and
            // last key of a row the focus should stay in that row instead.
            // Whether a key is at the edge is not guessed from the geometry
            // (rows have different margins and keys different widths): the press
            // is performed, and if it left the row it is undone with the
            // opposite direction and the focus walks to the other end of the
            // same row.
            if (key === Qt.Key_Left || key === Qt.Key_Right) {
                const keyboard = inputPanel.keyboard;
                const highlight = keyboard.navigationHighlight;
                const item = highlight ? highlight.highlightItem : null;
                if (keyboard.navigationModeActive && item && item !== keyboard) {
                    const rowHeight = keyboard.style ? keyboard.style.targetKeyboardHeight / 5 : keyboard.height / 5;
                    const rowY = keyboard.mapFromItem(item, 0, item.height / 2).y;
                    const inSameRow = (what) => what && what !== keyboard && Math.abs(keyboard.mapFromItem(what, 0, what.height / 2).y - rowY) < rowHeight * 0.6;
                    const press = (what) => {
                        inputPanel.InputContext.priv.navigationKeyPressed(what, false);
                        inputPanel.InputContext.priv.navigationKeyReleased(what, false);
                    };
                    const opposite = key === Qt.Key_Left ? Qt.Key_Right : Qt.Key_Left;

                    press(key);
                    if (!inSameRow(highlight.highlightItem)) {
                        // Wrapped onto the neighbouring row: go back and walk to
                        // the other end of this row.
                        press(opposite);
                        for (let i = 0; i < 24; ++i) {
                            press(opposite);
                            if (!inSameRow(highlight.highlightItem)) {
                                press(key);
                                break;
                            }
                        }
                    }
                    return;
                }
            }

            // The rows above the keyboard take part in the navigation: up from
            // the top row of the keyboard enters them, down from the bottom row
            // continues into them after a full circle.
            if (key === Qt.Key_Up || key === Qt.Key_Down) {
                const keyboard = inputPanel.keyboard;
                const highlight = keyboard.navigationHighlight;
                const item = highlight ? highlight.highlightItem : null;
                if (keyboard.navigationModeActive && item && item !== keyboard) {
                    const rowHeight = keyboard.style ? keyboard.style.targetKeyboardHeight / 5 : keyboard.height / 5;
                    const centreY = keyboard.mapFromItem(item, 0, item.height / 2).y;
                    const isTopRow = centreY < rowHeight;
                    const isBottomRow = centreY > keyboard.height - rowHeight;
                    const zone = key === Qt.Key_Up ? (isTopRow ? root.rowAbove(0) : 0) : (isBottomRow ? root.topExtraRow() : 0);
                    if (zone !== 0) {
                        // Keep the position: the focus lands on the item above
                        // (or below) the key that was selected.
                        const ratio = keyboard.mapFromItem(item, item.width / 2, 0).x / keyboard.width;
                        root.extraRowFocus = zone;
                        // The clipboard row always starts at its first entry.
                        root.extraColumn = zone === root.zoneOf("clipboard") ? 0 : root.columnForZone(zone, ratio);
                        return;
                    }

                }
            }

            inputPanel.InputContext.priv.navigationKeyPressed(key, false);
            inputPanel.InputContext.priv.navigationKeyReleased(key, false);
        }
        onActivate: {
            if (root.extraRowFocus !== 0) {
                root.activateExtraRowItem();
                return;
            }

            inputPanel.InputContext.priv.navigationKeyPressed(Qt.Key_Return, false);
            inputPanel.InputContext.priv.navigationKeyReleased(Qt.Key_Return, false);
        }
        onToggleExtraRows: {
            const zone = root.rowAbove(root.extraRowFocus);
            if (root.extraRowFocus === 0) {
                const keyboard = inputPanel.keyboard;
                const highlight = keyboard.navigationHighlight;
                const item = highlight ? highlight.highlightItem : null;
                const ratio = item && item !== keyboard ? keyboard.mapFromItem(item, item.width / 2, 0).x / keyboard.width : 0;
                root.extraColumn = zone === root.zoneOf("clipboard") ? 0 : root.columnForZone(zone, ratio);
            } else {
                root.extraColumn = root.columnForZone(zone, (root.extraColumn + 0.5) / root.extraRowItemCount(root.extraRowFocus));
            }
            root.extraRowFocus = zone;
        }
        onBackspace: thing.sendKeyEvent(Qt.Key_Backspace, "")
        onSpace: thing.sendKeyEvent(Qt.Key_Space, " ")
        onEnter: thing.sendKeyEvent(Qt.Key_Return, "\n")
        onToggleShift: inputPanel.InputContext.priv.shiftHandler.toggleShift()
        onToggleSymbols: inputPanel.keyboard.symbolMode = !inputPanel.keyboard.symbolMode
        onSwitchLanguage: inputPanel.keyboard.changeInputLanguage(false)
        onHideKeyboard: Qt.inputMethod.hide()
    }

    // Play the key click at full volume: the bundled sound is mastered quiet.
    Component.onCompleted: {
        VirtualKeyboardSettings.keySoundVolume = 100;

        // The navigation highlight of Qt Virtual Keyboard is animated, so while
        // the focus is moved into place it appears to travel through the keys it
        // passes. Without the animation it always shows the key that really has
        // the focus.
        if (inputPanel.keyboard.navigationHighlight) {
            inputPanel.keyboard.navigationHighlight.moveDuration = 0;
            inputPanel.keyboard.navigationHighlight.resizeDuration = 0;
        }
    }

    // Let the key panels know a gamepad is available, so they can show
    // the button glyphs directly on the relevant keys.
    Binding {
        target: PlasmaKeyboard.Modifiers
        property: "gamepadAvailable"
        value: gamepad.available
    }

    // While the gamepad is in the rows above the keyboard, the keyboard hides
    // its own navigation highlight (the item is kept, only the highlight goes).
    Binding {
        target: PlasmaKeyboard.Modifiers
        property: "extraRowsFocused"
        value: root.extraRowFocus !== 0
    }

    // Qt Virtual Keyboard's HideInputPanel only hides its internal panel; hide
    // our window as well so the keyboard actually disappears.
    Connections {
        target: inputPanel.InputContext.priv
        function onHideInputPanel() {
            Qt.inputMethod.hide();
        }
    }

    // Unified overlay system for diacritics, emoji, text expansion, etc.
    OverlayWindow {
        id: overlayWindow
        controller: thing.overlayController
        onCandidateSelected: (index) => thing.overlayController.commitCandidate(index)
    }

    interactiveRegion: Qt.rect(panelWrapper.x, panelWrapper.y, panelWrapper.width, panelWrapper.height)

    Kirigami.ShadowedRectangle {
        id: panelWrapper

        LanguagePopup {
            id: languageDialog
            style: inputPanel.keyboard.style
            keyboardPanel: inputPanel

            onShowSettings: root.showSettings()
        }

        // Whether the panel takes the full width of the screen
        readonly property bool isFullScreenWidth: PlasmaKeyboardSettings.panelFillScreenWidth

        color: PlasmaKeyboard.Theme.current.backgroundType === "gradient" ? "transparent" : PlasmaKeyboard.BreezeConstants.keyboardBackgroundColor

        // Themed gradient background, used when the theme asks for one. The
        // per-corner radii follow the panel's rounded corners.
        Rectangle {
            anchors.fill: parent
            visible: PlasmaKeyboard.Theme.current.backgroundType === "gradient"
            topLeftRadius: panelWrapper.corners.topLeftRadius
            topRightRadius: panelWrapper.corners.topRightRadius
            bottomLeftRadius: panelWrapper.corners.bottomLeftRadius
            bottomRightRadius: panelWrapper.corners.bottomRightRadius
            gradient: Gradient {
                orientation: PlasmaKeyboard.BreezeConstants.backgroundOrientation
                GradientStop { position: 0.0; color: PlasmaKeyboard.Theme.current.backgroundStart }
                GradientStop { position: 1.0; color: PlasmaKeyboard.Theme.current.backgroundEnd }
            }
        }

        // Provide shadow and radius when the keyboard is detached from edges
        corners {
            // The window isn't floating, so only curve the top
            bottomLeftRadius: Kirigami.Units.cornerRadius
            bottomRightRadius: Kirigami.Units.cornerRadius
            topLeftRadius: isFullScreenWidth ? 0 : Kirigami.Units.cornerRadius
            topRightRadius: isFullScreenWidth ? 0 : Kirigami.Units.cornerRadius
        }
        shadow {
            size: isFullScreenWidth ? 0 : 16
            color: Qt.rgba(0, 0, 0, 0.3)
        }

        // Starting x and y centers the panel on the bottom
        x: (root.width / 2) - (width / 2)
        y: root.height - height

        // Padding for background corners and panel drag area
        readonly property real padding: isFullScreenWidth ? 0 : Kirigami.Units.largeSpacing

        // Recent clipboard entries, read from the clipboard history of the
        // desktop while the feature is enabled in the settings. Tapping an
        // entry inserts it into the focused field. The row hides itself when
        // nothing has been copied yet.
        Item {
            id: clipboardRow

            readonly property var kbdStyle: inputPanel.keyboard.style
            // Half the height of a normal keyboard row, like the F-key row.
            readonly property real normalRowHeight: kbdStyle ? kbdStyle.targetKeyboardHeight / 5 : Kirigami.Units.gridUnit * 2
            readonly property real rowHeight: normalRowHeight / 2
            readonly property real fontScale: rowHeight / normalRowHeight
            readonly property real sideMargin: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin / 2
            // How many entries fit on screen at once: all entries have the same
            // width, longer texts are cut off.
            readonly property int visibleChips: 3
            readonly property real chipWidth: width / visibleChips

            visible: PlasmaKeyboardSettings.clipboardEnabled && thing.clipboardHistory.count > 0
            onVisibleChanged: {
                if (!visible && root.extraRowFocus === root.zoneOf("clipboard")) {
                    root.extraRowFocus = 0;
                }
            }

            anchors {
                top: parent.top
                topMargin: parent.padding
                horizontalCenter: parent.horizontalCenter
            }

            width: inputPanel.width > 0 ? inputPanel.width - sideMargin * 2 : 100
            height: visible ? rowHeight : 0

            //! Scrolls the entry selected with the gamepad into view.
            function showColumn(column) {
                const target = column * chipWidth + chipWidth / 2 - entries.width / 2;
                entries.contentX = Math.max(0, Math.min(entries.contentWidth - entries.width, target));
            }

            Connections {
                target: root
                function onExtraColumnChanged() {
                    if (root.extraRowFocus === root.zoneOf("clipboard")) {
                        clipboardRow.showColumn(root.extraColumn);
                    }
                }
                function onExtraRowFocusChanged() {
                    if (root.extraRowFocus === root.zoneOf("clipboard")) {
                        clipboardRow.showColumn(root.extraColumn);
                    }
                }
            }

            // The entries, flicked from the left edge. The clear button is not
            // part of this area, so it never scrolls away.
            Flickable {
                id: entries

                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width - clearButton.width
                height: parent.height

                contentWidth: chips.width
                contentHeight: height
                // The gaps between the chips are made by the key margins of the
                // cells, exactly like between the keyboard keys.
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                Row {
                    id: chips
                    height: entries.height
                    // Fewer entries than fit into the row: center them. More than
                    // fit: start at the left edge and let the row be flicked.
                    x: Math.max(0, (entries.width - width) / 2)

                    Repeater {
                        model: thing.clipboardHistory

                        delegate: Item {
                            id: clipboardChip
                            required property int index
                            required property string text

                            //! Selected with the gamepad.
                            readonly property bool focused: root.extraRowFocus === root.zoneOf("clipboard") && root.extraColumn === index

                            // A third of the row each, matching one third of the keyboard.
                            width: clipboardRow.chipWidth
                            height: clipboardRow.rowHeight

                            Kirigami.ShadowedRectangle {
                                id: chipBackground
                                // Same margins as a keyboard key, so the row looks like
                                // part of the keyboard.
                                anchors.fill: parent
                                anchors.margins: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin
                                radius: PlasmaKeyboard.BreezeConstants.buttonRadius

                                readonly property var outline: PlasmaKeyboard.Theme.current.keyOutlineFor("suggestions")
                                readonly property real shadowStrength: PlasmaKeyboard.Theme.current.keyShadowFor("suggestions")

                                color: PlasmaKeyboard.Theme.current.keyColorFor("suggestions", chipHandler.pressed ? "pressed" : "normal")

                                border.width: outline.width
                                border.color: outline.color
                                shadow.size: 3 * shadowStrength
                                shadow.yOffset: 1 * shadowStrength
                                shadow.color: Qt.rgba(0, 0, 0, 0.2 * shadowStrength)

                                // Selected with the gamepad: highlighted the way
                                // Qt Virtual Keyboard highlights its own keys.
                                Rectangle {
                                    anchors.fill: parent
                                    radius: PlasmaKeyboard.BreezeConstants.buttonRadius
                                    visible: clipboardChip.focused
                                    color: PlasmaKeyboard.BreezeConstants.navigationHighlightColor
                                    border.width: 2
                                    border.color: PlasmaKeyboard.BreezeConstants.navigationHighlightBorderColor
                                }

                                Text {
                                    id: label
                                    anchors.fill: parent
                                    anchors.leftMargin: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin * 2
                                    anchors.rightMargin: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin * 2

                                    verticalAlignment: Text.AlignVCenter
                                    // Entries can be multi-line: show them as a single line.
                                    text: clipboardChip.text.replace(/\s+/g, " ")
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    color: PlasmaKeyboard.BreezeConstants.keyTextColor
                                    font {
                                        family: PlasmaKeyboard.BreezeConstants.fontFamily
                                        pixelSize: clipboardRow.kbdStyle ? 60 * (clipboardRow.kbdStyle.targetKeyboardHeight / clipboardRow.kbdStyle.keyboardDesignHeight) * clipboardRow.fontScale : Kirigami.Units.gridUnit
                                    }
                                }
                            }

                            MouseArea {
                                id: chipHandler
                                anchors.fill: parent
                                onClicked: thing.commitText(clipboardChip.text)
                            }
                        }
                    }
                }
            }

            // Clearing the whole history: always in the right corner, as a
            // regular key (square, key background) next to the lighter entries.
            Item {
                id: clearButton

                //! Selected with the gamepad (the last column of the row).
                readonly property bool focused: root.extraRowFocus === root.zoneOf("clipboard") && root.extraColumn === thing.clipboardHistory.count

                anchors.right: parent.right
                anchors.top: parent.top
                width: clipboardRow.rowHeight
                height: clipboardRow.rowHeight

                Kirigami.ShadowedRectangle {
                    anchors.fill: parent
                    anchors.margins: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin
                    radius: PlasmaKeyboard.BreezeConstants.buttonRadius

                    readonly property var outline: PlasmaKeyboard.Theme.current.keyOutlineFor("normal")
                    readonly property real shadowStrength: PlasmaKeyboard.Theme.current.keyShadowFor("normal")

                    color: PlasmaKeyboard.Theme.current.keyColorFor("normal", clearHandler.pressed ? "pressed" : "normal")

                    border.width: outline.width
                    border.color: outline.color
                    shadow.size: 3 * shadowStrength
                    shadow.yOffset: 1 * shadowStrength
                    shadow.color: Qt.rgba(0, 0, 0, 0.2 * shadowStrength)

                    Rectangle {
                        anchors.fill: parent
                        radius: PlasmaKeyboard.BreezeConstants.buttonRadius
                        visible: clearButton.focused
                        color: PlasmaKeyboard.BreezeConstants.navigationHighlightColor
                        border.width: 2
                        border.color: PlasmaKeyboard.BreezeConstants.navigationHighlightBorderColor
                    }

                    Kirigami.Icon {
                        anchors.centerIn: parent
                        width: Math.round(clipboardRow.rowHeight * 0.6)
                        height: width
                        source: "edit-clear-history"
                        color: PlasmaKeyboard.BreezeConstants.keyTextColor
                    }
                }

                MouseArea {
                    id: clearHandler
                    anchors.fill: parent
                    onClicked: thing.clipboardHistory.clear()
                }
            }
        }

        // Optional F1-F12 row above the keyboard. The panel grows by the row
        // height when it is shown, the keyboard itself keeps its size.
        Row {
            id: functionKeyRow
            visible: PlasmaKeyboardSettings.showFunctionKeyRow
            // Match the keyboard geometry: five rows fill the keyboard height,
            // and the visible key background is inset by keyBackgroundMargin.
            readonly property var kbdStyle: inputPanel.keyboard.style
            // Half the height of a normal keyboard row.
            readonly property real normalRowHeight: kbdStyle ? kbdStyle.targetKeyboardHeight / 5 : Kirigami.Units.gridUnit * 2
            readonly property real keyHeight: normalRowHeight / 2
            // Key labels scale with the row height.
            readonly property real fontScale: keyHeight / normalRowHeight
            // Align the row with the keyboard keys below it: the keyboard
            // itself keeps a cell margin of about half a key margin.
            readonly property real sideMargin: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin / 2
            anchors {
                top: clipboardRow.visible ? clipboardRow.bottom : parent.top
                topMargin: parent.padding
                horizontalCenter: parent.horizontalCenter
            }
            width: inputPanel.width > 0 ? inputPanel.width - sideMargin * 2 : 100
            height: visible ? keyHeight : 0

            Repeater {
                model: 12
                delegate: Item {
                    required property int index
                    width: functionKeyRow.width / 12
                    height: functionKeyRow.keyHeight

                    Kirigami.ShadowedRectangle {
                        anchors.fill: parent
                        anchors.margins: PlasmaKeyboard.BreezeConstants.keyBackgroundMargin
                        radius: PlasmaKeyboard.BreezeConstants.buttonRadius
                        readonly property bool focused: root.extraRowFocus === root.zoneOf("fkeys") && root.extraColumn === index
                        readonly property var outline: PlasmaKeyboard.Theme.current.keyOutlineFor("function")
                        readonly property real shadowStrength: PlasmaKeyboard.Theme.current.keyShadowFor("function")

                        color: PlasmaKeyboard.Theme.current.hasCategoryColors("function")
                            ? PlasmaKeyboard.Theme.current.keyColorFor("function", pressHandler.pressed ? "pressed" : "normal")
                            : (pressHandler.pressed ? PlasmaKeyboard.BreezeConstants.primaryDarkColor : PlasmaKeyboard.BreezeConstants.normalKeyBackgroundColor)

                        border.width: outline.width
                        border.color: outline.color
                        shadow.size: 3 * shadowStrength
                        shadow.yOffset: 1 * shadowStrength
                        shadow.color: Qt.rgba(0, 0, 0, 0.2 * shadowStrength)

                        Rectangle {
                            anchors.fill: parent
                            radius: PlasmaKeyboard.BreezeConstants.buttonRadius
                            visible: parent.focused
                            color: PlasmaKeyboard.BreezeConstants.navigationHighlightColor
                            border.width: 2
                            border.color: PlasmaKeyboard.BreezeConstants.navigationHighlightBorderColor
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "F" + (index + 1)
                            color: PlasmaKeyboard.Theme.current.keyTextColorFor("function")
                            font {
                                family: PlasmaKeyboard.BreezeConstants.fontFamily
                                weight: Font.Bold
                                // Keyboard label size, scaled with the row height.
                                pixelSize: functionKeyRow.kbdStyle ? 60 * (functionKeyRow.kbdStyle.targetKeyboardHeight / functionKeyRow.kbdStyle.keyboardDesignHeight) * functionKeyRow.fontScale : Kirigami.Units.gridUnit
                            }
                        }
                    }

                    MouseArea {
                        id: pressHandler
                        anchors.fill: parent
                        onClicked: thing.sendKeyEvent(Qt.Key_F1 + index, "")
                    }
                }
            }
        }

        // Never let width & height to be 0, otherwise it can cause problems for setting interactiveRegion
        width: inputPanel.width > 0 ? (inputPanel.width + padding * 2) : 100
        height: inputPanel.height > 0
            ? (inputPanel.height + padding * 2 + (functionKeyRow.visible ? functionKeyRow.height : 0) + (clipboardRow.visible ? clipboardRow.height : 0))
            : 100

        InputPanel {
            id: inputPanel
            anchors {
                top: functionKeyRow.visible ? functionKeyRow.bottom : (clipboardRow.visible ? clipboardRow.bottom : parent.top)
                // Keep the vertical rhythm of the keyboard rows when the F-key
                // row is shown.
                topMargin: functionKeyRow.visible ? -functionKeyRow.sideMargin : parent.padding
                left: parent.left
                leftMargin: parent.padding
            }

            // height is calculated by InputPanel
            width: inputPanel.keyboard.style ? inputPanel.keyboard.style.aspectRatio * inputPanel.keyboard.style.targetKeyboardHeight : 0

            focusPolicy: Qt.NoFocus
            externalLanguageSwitchEnabled: true
            onExternalLanguageSwitch: (localeList, currentIndex) => {
                languageDialog.show(inputPanel.keyboard.activeKey, localeList, currentIndex)
            }

            function updateLocales() {
                if (PlasmaKeyboardSettings.enabledLocales.length === 0) {
                    // If there are no enabled locales, set it to the current locale
                    // NOTE: If Qt.locale().name is not valid, then all keyboard layouts will be shown.
                    let locale = Qt.locale().name;
                    if (locale === "C") {
                        locale = "en_US";
                    }
                    VirtualKeyboardSettings.activeLocales = [locale];
                } else {
                    VirtualKeyboardSettings.activeLocales = PlasmaKeyboardSettings.enabledLocales;
                }
            }

            Connections {
                target: VirtualKeyboardSettings
                function onAvailableLocalesChanged() {
                    inputPanel.updateLocales();
                }
            }

            Connections {
                target: PlasmaKeyboardSettings
                function onEnabledLocalesChanged() {
                    inputPanel.updateLocales();
                }
                function onThemeChanged() {
                    PlasmaKeyboard.Theme.setThemeId(PlasmaKeyboardSettings.theme);
                }
            }

            Component.onCompleted: {
                VirtualKeyboardSettings.styleName = "PlasmaBreeze";
                PlasmaKeyboard.Theme.setThemeId(PlasmaKeyboardSettings.theme);
                // Enable Qt Virtual Keyboard's arrow-key navigation so the
                // gamepad can move the highlight and activate keys.
                VirtualKeyboardSettings.arrowKeyNavigationEnabled = true;
                inputPanel.updateLocales();
            }
        }
    }

    // Steam-like button prompt on the key that is currently focused.
    Item {
        id: gamepadKeyPrompt
        readonly property Item activeKey: inputPanel.keyboard ? inputPanel.keyboard.activeKey : null
        visible: gamepad.available && inputPanel.keyboard.navigationModeActive && activeKey !== null
        width: 24
        height: 24
        x: activeKey ? activeKey.mapToItem(null, activeKey.width - width - 3, activeKey.height - height - 3).x : 0
        y: activeKey ? activeKey.mapToItem(null, activeKey.width - width - 3, activeKey.height - height - 3).y : 0

        Rectangle {
            anchors.fill: parent
            radius: width / 2
            color: "#2e7d32"
            border.color: "white"
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "A"
                color: "white"
                font.bold: true
                font.pixelSize: 15
            }
        }
    }
}
