// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <qqmlregistration.h>

#include "hapticinterface.h"
#include "vibrationevent.h"

class Vibration : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit Vibration(QObject *parent = nullptr);

    Q_INVOKABLE void vibrate(int durationMs, int strengthPercent = 75);

private:
    // InputPlumber (used on handhelds) exposes the haptics of the gamepad.
    bool rumbleGamepad(int durationMs, double strength);
    void stopRumble();
    // Fallback for systems providing the org.sigxcpu.Feedback service.
    void vibrateFeedbackd(int durationMs, double strength);

    OrgSigxcpuFeedbackHapticInterface *m_interface{nullptr};
    QString m_forceFeedbackPath;
    QTimer m_stopTimer;
};
