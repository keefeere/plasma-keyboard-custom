/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "touchholdwatcher.h"

#include "logging.h"

#include <QDir>
#include <QSocketNotifier>

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

namespace
{
// The touchscreen is identified by having both BTN_TOUCH and
// ABS_MT_POSITION_X capabilities, plus INPUT_PROP_DIRECT (see isDirectInput()).
bool hasCapability(int fd, int type, unsigned int code)
{
    unsigned char bits[512];
    memset(bits, 0, sizeof(bits));
    if (ioctl(fd, EVIOCGBIT(type, sizeof(bits)), bits) < 0) {
        return false;
    }
    return bits[code / 8] & (1 << (code % 8));
}

// Touchpads can have BTN_TOUCH and ABS_MT too (e.g. InputPlumber's virtual
// touchpad on handhelds); only a touchscreen is a direct input device.
bool isDirectInput(int fd)
{
    unsigned char props[INPUT_PROP_CNT / 8 + 1];
    memset(props, 0, sizeof(props));
    if (ioctl(fd, EVIOCGPROP(sizeof(props)), props) < 0) {
        return false;
    }
    return props[INPUT_PROP_DIRECT / 8] & (1 << (INPUT_PROP_DIRECT % 8));
}

// A hold may wobble slightly, but scrolling and dragging should cancel it.
// Prefer a physical distance when the device reports its resolution; the
// range-based fallback is for touchscreens whose evdev resolution is missing.
int movementSlop(int fd, unsigned int code)
{
    input_absinfo info{};
    if (ioctl(fd, EVIOCGABS(code), &info) < 0) {
        return 1;
    }
    constexpr int slopMillimetres = 8;
    if (info.resolution > 0) {
        return std::max(1, info.resolution * slopMillimetres);
    }
    return std::max(1, (info.maximum - info.minimum) / 100);
}
}

TouchHoldWatcher::TouchHoldWatcher(QObject *parent)
    : QObject(parent)
{
    // Start observing before an input context is activated. Opening the device
    // from arm() is too late for the first touch: the BTN_TOUCH press that
    // focused the text field has already been delivered by then, so the watcher
    // only sees its release and can never recognise that first long press.
    // Failure is harmless; arm() retries in case permissions or devices change.
    openTouchscreen();

    connect(&m_holdTimer, &QTimer::timeout, this, [this] {
        if (m_armed && m_touchDown && !m_gestureMoved && m_activeSlots.size() <= 1) {
            // One activation arms exactly one touch. Keeping this armed after
            // success would make a still-focused text field turn every later
            // long press anywhere on the screen into a keyboard request.
            disarm();
            Q_EMIT longPress();
        }
    });
}

TouchHoldWatcher::~TouchHoldWatcher()
{
    delete m_notifier;
    if (m_fd >= 0) {
        ::close(m_fd);
    }
}

bool TouchHoldWatcher::openTouchscreen()
{
    const auto devices = QDir(QStringLiteral("/dev/input")).entryList({QStringLiteral("event*")}, QDir::System, QDir::Name);
    for (const QString &name : devices) {
        const QString path = QStringLiteral("/dev/input/") + name;
        const int fd = ::open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);
        if (fd < 0) {
            continue;
        }
        if (hasCapability(fd, EV_ABS, ABS_MT_POSITION_X) && hasCapability(fd, EV_KEY, BTN_TOUCH) && isDirectInput(fd)) {
            m_fd = fd;
            m_xSlop = movementSlop(fd, ABS_MT_POSITION_X);
            m_ySlop = movementSlop(fd, ABS_MT_POSITION_Y);
            m_notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
            connect(m_notifier, &QSocketNotifier::activated, this, &TouchHoldWatcher::readEvents);
            return true;
        }
        ::close(fd);
    }

    qCWarning(PlasmaKeyboard) << "TouchHoldWatcher: no readable touchscreen device found";
    return false;
}

bool TouchHoldWatcher::arm(int thresholdMs)
{
    if (m_fd < 0 && !openTouchscreen()) {
        return false;
    }

    // The Wayland activation and evdev fd are separate event sources. Drain
    // evdev here so touchIsDown() is correct even when Wayland happened to be
    // dispatched first for the touch that focused the field.
    readEvents();

    m_thresholdMs = thresholdMs;
    m_armed = true;
    qCDebug(PlasmaKeyboard) << "TouchHoldWatcher armed, touchscreen fd=" << m_fd << "touchDown=" << m_touchDown;
    if (m_touchDown && !m_gestureMoved && m_activeSlots.size() <= 1) {
        m_holdTimer.start(m_thresholdMs);
    }
    return true;
}

void TouchHoldWatcher::disarm()
{
    m_armed = false;
    m_holdTimer.stop();
}

bool TouchHoldWatcher::isArmed() const
{
    return m_armed;
}

bool TouchHoldWatcher::touchIsDown() const
{
    return m_touchDown;
}

void TouchHoldWatcher::cancelGesture()
{
    m_gestureMoved = true;
    m_holdTimer.stop();
}

void TouchHoldWatcher::updatePosition(int code, int value)
{
    if (code == ABS_MT_POSITION_X) {
        m_x = value;
        m_currentXValid = true;
        if (!m_touchDown) {
            return;
        }
        if (!m_haveX) {
            m_startX = value;
            m_haveX = true;
        }
    } else if (code == ABS_MT_POSITION_Y) {
        m_y = value;
        m_currentYValid = true;
        if (!m_touchDown) {
            return;
        }
        if (!m_haveY) {
            m_startY = value;
            m_haveY = true;
        }
    }

    if ((m_haveX && std::abs(m_x - m_startX) > m_xSlop) || (m_haveY && std::abs(m_y - m_startY) > m_ySlop)) {
        cancelGesture();
    }
}

void TouchHoldWatcher::readEvents()
{
    struct input_event events[16];

    while (true) {
        const ssize_t n = ::read(m_fd, events, sizeof(events));
        if (n < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                break;
            }
            // The device is gone (re-plugged, permissions changed); stop reading.
            delete m_notifier;
            m_notifier = nullptr;
            ::close(m_fd);
            m_fd = -1;
            break;
        }
        if (n == 0) {
            break;
        }

        const auto count = static_cast<size_t>(n) / sizeof(events[0]);
        for (size_t i = 0; i < count; ++i) {
            const input_event &ev = events[i];

            if (ev.type == EV_ABS) {
                if (ev.code == ABS_MT_SLOT) {
                    m_currentSlot = ev.value;
                } else if (ev.code == ABS_MT_TRACKING_ID) {
                    if (ev.value >= 0) {
                        m_activeSlots.insert(m_currentSlot);
                        if (m_activeSlots.size() > 1) {
                            cancelGesture();
                        }
                    } else {
                        m_activeSlots.remove(m_currentSlot);
                    }
                } else if (ev.code == ABS_MT_POSITION_X || ev.code == ABS_MT_POSITION_Y) {
                    updatePosition(ev.code, ev.value);
                }
                continue;
            }

            if (ev.type != EV_KEY || ev.code != BTN_TOUCH) {
                continue;
            }

            if (ev.value == 1) {
                m_touchDown = true;
                m_gestureMoved = false;
                m_haveX = m_currentXValid;
                m_haveY = m_currentYValid;
                m_startX = m_x;
                m_startY = m_y;
                if (m_armed) {
                    m_holdTimer.start(m_thresholdMs);
                }
            } else if (ev.value == 0) {
                m_touchDown = false;
                m_activeSlots.clear();
                m_currentXValid = false;
                m_currentYValid = false;
                // A short tap, scroll or drag must not leave the watcher ready
                // to accept an unrelated touch elsewhere on the screen.
                disarm();
            }
        }
    }
}
