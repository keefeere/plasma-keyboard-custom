/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QTimer>

class QSocketNotifier;

/**
 * Watches the touchscreen evdev device for touch down/up events.
 *
 * When armed, it emits longPress() once a touch is held down for longer
 * than the configured threshold. Used to open the keyboard only after a
 * deliberate long press instead of immediately when a text field is
 * focused.
 */
class TouchHoldWatcher : public QObject
{
    Q_OBJECT

public:
    static constexpr int defaultThresholdMs = 600;

    explicit TouchHoldWatcher(QObject *parent = nullptr);
    ~TouchHoldWatcher() override;

    TouchHoldWatcher(const TouchHoldWatcher &) = delete;
    TouchHoldWatcher &operator=(const TouchHoldWatcher &) = delete;

    /**
     * Start watching for a long touch.
     *
     * @param thresholdMs How long the touch must be held to emit longPress().
     * @return False if no usable touchscreen could be opened, in which case
     * the caller should fall back to its normal behaviour.
     */
    bool arm(int thresholdMs = defaultThresholdMs);

    /// Stop watching; any pending hold timer is cancelled.
    void disarm();

    bool isArmed() const;

    /// Whether a touch is currently down on the touchscreen.
    bool touchIsDown() const;

Q_SIGNALS:
    /// A touch was held down for at least the threshold duration.
    void longPress();

private:
    bool openTouchscreen();
    void readEvents();

    int m_fd = -1;
    QSocketNotifier *m_notifier = nullptr;
    bool m_touchDown = false;
    bool m_armed = false;
    int m_thresholdMs = defaultThresholdMs;
    QTimer m_holdTimer;
};
