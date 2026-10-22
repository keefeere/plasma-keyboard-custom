/*
    SPDX-FileCopyrightText: 2026 plasma-keyboard-custom contributors
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include <LayerShellQt/Window>
#include <QGuiApplication>
#include <QTest>
#include <QWindow>

class LayerShellOutputPolicyTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void explicitScreenOverridesActiveOutput()
    {
        QWindow window;
        auto *layer = LayerShellQt::Window::get(&window);
        QVERIFY(layer);
        QVERIFY(QGuiApplication::primaryScreen());
        layer->setWantsToBeOnActiveScreen(true);
        QVERIFY(layer->wantsToBeOnActiveScreen());
        QVERIFY(!layer->screen());
        // This is the application's startup -> syncKeyboardScreen sequence.
        layer->setScreen(QGuiApplication::primaryScreen());
        QVERIFY(!layer->wantsToBeOnActiveScreen());
        QCOMPARE(layer->screen(), QGuiApplication::primaryScreen());
        QVERIFY(!window.handle()); // Property contract only, no native surface.
    }

    void activeOutputClearsOldPin()
    {
        QWindow window;
        auto *layer = LayerShellQt::Window::get(&window);
        layer->setScreen(QGuiApplication::primaryScreen());
        layer->setWantsToBeOnActiveScreen(true);
        QVERIFY(layer->wantsToBeOnActiveScreen());
        QVERIFY(!layer->screen());
        QVERIFY(!window.handle());
    }
};
QTEST_MAIN(LayerShellOutputPolicyTest)
#include "layershelloutputpolicytest.moc"
