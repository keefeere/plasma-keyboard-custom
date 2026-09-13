/*
    SPDX-FileCopyrightText: 2024 Aleix Pol i Gonzalez <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "config-plasma-keyboard.h"
#include "inputlisteneritem.h"
#include "inputpanelintegration.h"
#include "layoutpathhelper.h"
#include "logging.h"
#include "plasmakeyboardsettings.h"
#include <plasma_keyboard_version.h>

#include <KAboutData>
#include <KConfigWatcher>
#include <KCrash>
#include <KGlobalAccel>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <QAction>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QWindow>
#include <qpa/qwindowsysteminterface.h>

// signal handler for SIGINT & SIGTERM
#ifdef Q_OS_UNIX
#include <KSignalHandler>
#include <signal.h>
#include <unistd.h>
#endif

int main(int argc, char **argv)
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    initLayoutsPath();

    QGuiApplication application(argc, argv);

    KLocalizedString::setApplicationDomain("plasma-keyboard");

    KAboutData aboutData(QStringLiteral("plasma-keyboard-custom"),
                         i18n("Plasma Keyboard"),
                         QStringLiteral(PLASMA_KEYBOARD_VERSION_STRING),
                         i18n("An on-screen keyboard for Plasma"),
                         KAboutLicense::GPL,
                         i18n("Copyright 2024, Aleix Pol Gonzalez"));

    aboutData.addAuthor(i18n("Aleix Pol Gonzalez"), i18n("Author"), QStringLiteral("aleixpol@kde.org"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.plasma.keyboard.custom"));
    application.setWindowIcon(QIcon::fromTheme(QStringLiteral("input-keyboard-virtual")));
    aboutData.setProgramLogo(application.windowIcon());

    KAboutData::setApplicationData(aboutData);

    // Global shortcut to open the keyboard. It is configurable in
    // System Settings -> Shortcuts -> Plasma Keyboard (custom).
    auto *showAction = new QAction(&application);
    showAction->setObjectName(QStringLiteral("show-virtual-keyboard"));
    showAction->setText(i18n("Show Virtual Keyboard"));
    showAction->setProperty("componentName", QStringLiteral("org.kde.plasma.keyboard.custom"));
    showAction->setProperty("componentDisplayName", i18n("Plasma Keyboard (custom)"));
    const QList<QKeySequence> defaultShortcut{QKeySequence(Qt::META | Qt::SHIFT | Qt::Key_K)};
    KGlobalAccel::self()->setDefaultShortcut(showAction, defaultShortcut, KGlobalAccel::NoAutoloading);
    KGlobalAccel::self()->setShortcut(showAction, defaultShortcut, KGlobalAccel::NoAutoloading);
    QObject::connect(showAction, &QAction::triggered, &application, [] {
        qCDebug(PlasmaKeyboard) << "Show-virtual-keyboard shortcut triggered";
        QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                          QStringLiteral("/VirtualKeyboard"),
                                                          QStringLiteral("org.kde.kwin.VirtualKeyboard"),
                                                          QStringLiteral("forceActivate"));
        QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
    });

    KCrash::initialize();

    {
        QCommandLineParser parser;
        aboutData.setupCommandLine(&parser);
        parser.process(application);
        aboutData.processCommandLine(&parser);
    }

    if (!PLASMA_KEYBOARD_SOUND_ENABLED) {
        PlasmaKeyboardSettings::self()->setSoundEnabled(false);
    }

    if (!PLASMA_KEYBOARD_VIBRATION_ENABLED) {
        PlasmaKeyboardSettings::self()->setVibrationEnabled(false);
    }

    // Listen to config updates from kcm, and reparse
    auto watcher = KConfigWatcher::create(PlasmaKeyboardSettings::self()->sharedConfig());
    // clang-format off
    QObject::connect(watcher.get(),
        &KConfigWatcher::configChanged,
        &application,
        [](const KConfigGroup &, const QByteArrayList &) {
            PlasmaKeyboardSettings::self()->sharedConfig()->reparseConfiguration();
            PlasmaKeyboardSettings::self()->load();
        });
    // clang-format on

    // Expose the Ctrl/Alt latch state to the keyboard layouts.
    qmlRegisterSingletonInstance("org.kde.plasma.keyboard.custom.lib", 1, 0, "Modifiers", KeyboardModifiers::instance());

    QQmlApplicationEngine view;
    KLocalization::setupLocalizedContext(&view);

    QObject::connect(&view, &QQmlApplicationEngine::objectCreated, &application, [](QObject *object) {
        auto window = qobject_cast<QWindow *>(object);
        const bool initSuccessful = initInputPanelIntegration(window, InputPanelRole::Keyboard);

        if (!initSuccessful) {
            qCCritical(PlasmaKeyboard)
                << "Cannot run plasma-keyboard-custom standalone. You can enable it in Plasma's System Settings app, on the “Virtual Keyboard” page.";
            exit(1);
        }

        window->requestActivate();
        window->setVisible(true);
    });
    view.load(QUrl(QStringLiteral("qrc:/qt/qml/org/kde/plasma/keyboard/custom/main.qml")));

#ifdef Q_OS_UNIX
    /**
     * Set up signal handler for SIGINT and SIGTERM
     */
    KSignalHandler::self()->watchSignal(SIGINT);
    KSignalHandler::self()->watchSignal(SIGTERM);
    QObject::connect(KSignalHandler::self(), &KSignalHandler::signalReceived, &application, [](int signal) {
        if (signal == SIGINT || signal == SIGTERM) {
            qCDebug(PlasmaKeyboard) << "Received signal" << signal << ", exiting now.";
            QCoreApplication::quit();
        }
    });
#endif

    qCDebug(PlasmaKeyboard) << "Starting Plasma Keyboard application";

    return application.exec();
}
