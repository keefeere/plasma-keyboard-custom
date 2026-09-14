/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "touchholdwatcher.h"
#include <QQuickItem>
#include <QQuickWindow>
#include <QVirtualKeyboardInputEngine>
#include <qqmlintegration.h>

#include <xkbcommon/xkbcommon.h>

#include "inputplugin.h"

class OverlayController;

/**
 * Global state for the on-screen Ctrl/Alt keys.
 *
 * The keys in the layouts toggle these values. InputListenerItem consults
 * them when forwarding key events to the compositor, so that a latched
 * modifier is applied to the next key and then cleared.
 */
class KeyboardModifiers : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ctrl READ ctrl WRITE setCtrl NOTIFY ctrlChanged)
    Q_PROPERTY(bool alt READ alt WRITE setAlt NOTIFY altChanged)
    //! True while a gamepad is available, so key panels can show button hints.
    Q_PROPERTY(bool gamepadAvailable READ gamepadAvailable WRITE setGamepadAvailable NOTIFY gamepadAvailableChanged)

public:
    static KeyboardModifiers *instance();

    bool ctrl() const;
    void setCtrl(bool ctrl);

    bool alt() const;
    void setAlt(bool alt);

    bool gamepadAvailable() const;
    void setGamepadAvailable(bool available);

    Q_INVOKABLE void reset();

Q_SIGNALS:
    void ctrlChanged();
    void altChanged();
    void gamepadAvailableChanged();

private:
    explicit KeyboardModifiers(QObject *parent = nullptr);

    bool m_ctrl = false;
    bool m_alt = false;
    bool m_gamepadAvailable = false;
};

/**
 * Requests the keyboard to be shown on the next input activation, bypassing
 * the "open on long press" behaviour. Used by the global shortcut, which
 * force-activates the input method and would otherwise be suppressed.
 */
void setInputPanelForceShowOnNextActivation();

class InputListenerItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVirtualKeyboardInputEngine *engine WRITE setEngine)
    Q_PROPERTY(bool keyboardNavigationActive MEMBER m_keyboardNavigationActive)

    /**
     * Controller for overlay popups (diacritics, emoji, text expansion).
     *
     * Exposed to QML for connecting overlay windows.
     */
    Q_PROPERTY(OverlayController *overlayController READ overlayController CONSTANT)

public:
    InputListenerItem();

    void setEngine(QVirtualKeyboardInputEngine *engine);

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;

    /**
     * Synthesizes a key press/release pair, as if it came from the virtual
     * keyboard. Used by the gamepad handler to emit e.g. Backspace/Space.
     */
    Q_INVOKABLE void sendKeyEvent(int key, const QString &text);

    /**
     * Get the overlay controller.
     */
    OverlayController *overlayController() const;

Q_SIGNALS:
    void keyNavigationPressed(int key);
    void keyNavigationReleased(int key);

private:
    /**
     * Sends the key described by @p event as a real key event with the
     * currently latched Ctrl/Alt modifiers applied.
     *
     * Returns true if the event was handled, in which case the caller must
     * not process it any further.
     */
    bool handleModifiedKey(QKeyEvent *event, bool press);

    InputPlugin m_input;
    OverlayController *m_overlayController = nullptr;
    TouchHoldWatcher m_touchHold;
    bool m_keyboardNavigationActive = false;
};
