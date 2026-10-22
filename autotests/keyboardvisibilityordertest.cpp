/*
    SPDX-FileCopyrightText: 2026 plasma-keyboard-custom contributors
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include <QQuickWindow>
#include <QTest>

class KeyboardVisibilityOrderTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void authorizationMustBePostShow()
    {
        QQuickWindow window;
        window.resize(160, 100);
        bool earlySignal = false;
        bool showCompleted = false;
        bool earlyHadPlatformWindow = true;
        connect(&window, &QWindow::visibleChanged, this, [&](bool visible) {
            if (visible) {
                earlySignal = true;
                QVERIFY(!showCompleted);
                earlyHadPlatformWindow = window.handle() != nullptr;
            }
        });
        window.show();
        showCompleted = true;
        QVERIFY(earlySignal);
        QVERIFY(!earlyHadPlatformWindow);
        QVERIFY(window.handle());
        // A caller following QWindow::show() can initialize/authorize the role;
        // a visibleChanged(true) handler cannot infer native readiness.
        window.hide();
    }
};
QTEST_MAIN(KeyboardVisibilityOrderTest)
#include "keyboardvisibilityordertest.moc"
