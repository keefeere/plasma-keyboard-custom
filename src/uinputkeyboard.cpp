/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "uinputkeyboard.h"
#include "logging.h"

#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <linux/uinput.h>
#include <thread>
#include <unistd.h>

UinputKeyboard::UinputKeyboard()
{
    m_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (m_fd < 0) {
        qCWarning(PlasmaKeyboard) << "Could not open /dev/uinput, Meta shortcuts will not reach the compositor's global shortcuts:"
                                   << strerror(errno);
        return;
    }

    ioctl(m_fd, UI_SET_EVBIT, EV_KEY);
    // The full standard keycode range (Escape through the multimedia keys),
    // rather than hand-picking one key at a time for every combination this
    // might ever need to send.
    for (int key = 1; key <= 248; ++key) {
        ioctl(m_fd, UI_SET_KEYBIT, key);
    }

    uinput_setup setup{};
    std::strncpy(setup.name, "plasma-keyboard-custom Meta", UINPUT_MAX_NAME_SIZE - 1);
    setup.id.bustype = BUS_VIRTUAL;
    setup.id.vendor = 0x0000;
    setup.id.product = 0x0000;
    setup.id.version = 1;

    if (ioctl(m_fd, UI_DEV_SETUP, &setup) < 0 || ioctl(m_fd, UI_DEV_CREATE) < 0) {
        qCWarning(PlasmaKeyboard) << "Could not create the uinput virtual keyboard, Meta shortcuts will not reach the compositor's global shortcuts:"
                                   << strerror(errno);
        close(m_fd);
        m_fd = -1;
    }
}

UinputKeyboard::~UinputKeyboard()
{
    if (m_fd >= 0) {
        ioctl(m_fd, UI_DEV_DESTROY);
        close(m_fd);
    }
}

bool UinputKeyboard::isValid() const
{
    return m_fd >= 0;
}

void UinputKeyboard::emit(int keyCode, int value)
{
    if (m_fd < 0) {
        return;
    }

    input_event event{};
    event.type = EV_KEY;
    event.code = static_cast<uint16_t>(keyCode);
    event.value = value;
    if (write(m_fd, &event, sizeof(event)) != sizeof(event)) {
        qCWarning(PlasmaKeyboard) << "Failed to write a uinput key event:" << strerror(errno);
        return;
    }

    input_event sync{};
    sync.type = EV_SYN;
    sync.code = SYN_REPORT;
    sync.value = 0;
    if (write(m_fd, &sync, sizeof(sync)) != sizeof(sync)) {
        qCWarning(PlasmaKeyboard) << "Failed to write a uinput sync event:" << strerror(errno);
    }

    // Without a pause here, a combination's events (e.g. Meta down, then L
    // down a function call later but still the same millisecond) can reach
    // libinput/KWin close enough together that the compositor's global
    // shortcut matcher evaluates the second key before it has finished
    // registering the first one as held -- a real race, not a hypothetical
    // one: it showed up as "Meta+L locks the session, then the lock
    // immediately flies back open" (actually the launcher opening from a
    // Meta seen as a bare tap, then losing focus again right away). A human
    // pressing a physical combo always leaves tens of milliseconds of overlap
    // between the keys for the same reason. 10ms per event is well under
    // that and not perceptible when typing.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void UinputKeyboard::pressKey(int keyCode)
{
    emit(keyCode, 1);
}

void UinputKeyboard::releaseKey(int keyCode)
{
    emit(keyCode, 0);
}
