/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

/**
 * A minimal virtual keyboard on /dev/uinput.
 *
 * The Wayland input-method-v1 protocol this application otherwise talks to
 * the compositor over only delivers keys to whichever client currently holds
 * a text input focus -- it was never meant to reach the compositor's own
 * global shortcuts (Meta+L to lock the session, Meta+D to show the desktop,
 * the bare Meta tap that opens the application launcher, Alt+Tab to switch
 * windows; see ~/.config/kglobalshortcutsrc). Those are matched against real
 * input, which a uinput device provides: the kernel (and therefore libinput
 * and KWin) sees it exactly like a hardware keyboard, so a key sent through
 * it can reach global shortcuts that input-method-v1 cannot.
 *
 * Used for every Ctrl/Alt/Meta combination (see InputListenerItem), so it
 * also covers plain application-level shortcuts like Ctrl+C -- that keeps
 * working the same, since the compositor forwards a real key press to
 * whichever client has keyboard focus, same as input-method-v1 did for that
 * case. It is only a fallback to the input-method-v1 path (kept for when
 * /dev/uinput cannot be opened) that is now the exception rather than the
 * rule.
 */
class UinputKeyboard
{
public:
    UinputKeyboard();
    ~UinputKeyboard();

    UinputKeyboard(const UinputKeyboard &) = delete;
    UinputKeyboard &operator=(const UinputKeyboard &) = delete;

    //! False if /dev/uinput could not be opened or the virtual device could
    //! not be created (e.g. no access, no uinput module); callers must treat
    //! that as "the feature is unavailable" rather than an error.
    bool isValid() const;

    //! Sends a key-down for the given Linux evdev key code (KEY_* from
    //! <linux/input-event-codes.h>) and syncs the device. No-op if !isValid().
    void pressKey(int keyCode);

    //! Sends a key-up for the given evdev key code and syncs the device.
    //! No-op if !isValid().
    void releaseKey(int keyCode);

private:
    void emit(int keyCode, int value);

    int m_fd = -1;
};
