/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QSet>
#include <QString>

#include <qqmlintegration.h>

class QTimer;

/**
 * Translates gamepad input into on-screen keyboard actions.
 *
 * On systems using InputPlumber, the built-in controller is owned by the
 * InputPlumber daemon, which exposes its events on the system bus through an
 * "dbus" target device (interface org.shadowblip.Input.DBusDevice, signal
 * InputEvent(string, double)). This class subscribes to that stream and emits
 * high level signals that the keyboard UI maps onto Qt Virtual Keyboard's
 * navigation and input.
 *
 * When InputPlumber is not available the handler stays idle.
 */
class GamepadHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /*! Whether an InputPlumber dbus target device was found. */
    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)

public:
    explicit GamepadHandler(QObject *parent = nullptr);
    ~GamepadHandler() override;

    bool isAvailable() const;

    /**
     * While active (keyboard shown), gamepad input is intercepted by
     * InputPlumber and no longer reaches the game/Steam, so that the buttons
     * only drive the on-screen keyboard. The previous intercept mode is
     * restored when deactivated.
     */
    Q_INVOKABLE void setActive(bool active);
    bool isActive() const;

Q_SIGNALS:
    /*! Move the keyboard focus in the given direction (Qt::Key_Up/Down/Left/Right). */
    void navigate(int key);
    /*! Type the currently highlighted key. */
    void activate();
    /*! Delete the character before the cursor. */
    void backspace();
    /*! Insert a space. */
    void space();
    /*! Press Enter/Return. */
    void enter();
    /*! Toggle the shift state. */
    void toggleShift();
    /*! Toggle the symbols (&123) layer. */
    void toggleSymbols();
    /*! Switch to the next keyboard layout. */
    void switchLanguage();
    /*! Hide the keyboard. */
    void hideKeyboard();

    void availableChanged();

private Q_SLOTS:
    void onInputEvent(const QString &event, double value);

private:
    void handleDirection(int key, bool pressed);
    uint interceptMode() const;
    void setInterceptMode(uint mode);

    bool m_available = false;
    QString m_compositePath;
    bool m_active = false;
    uint m_savedInterceptMode = 0;
    QSet<int> m_pressedDirections;
    QTimer *m_repeatTimer = nullptr;
    int m_repeatKey = 0;
};
