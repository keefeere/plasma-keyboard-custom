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
#include <KConfig>
#include <KConfigGroup>
#include <KConfigWatcher>
#include <KCrash>
#include <KGlobalAccel>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <QAction>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QDir>
#include <QGuiApplication>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QStandardPaths>
#include <QTimer>
#include <QVariantMap>
#include <QWindow>
#include <qpa/qwindowsysteminterface.h>

namespace
{
constexpr auto s_kwinService = "org.kde.KWin";
constexpr auto s_kwinPath = "/VirtualKeyboard";
constexpr auto s_kwinIface = "org.kde.kwin.VirtualKeyboard";
constexpr auto s_kwinPropertiesIface = "org.freedesktop.DBus.Properties";

int kwinMode()
{
    QDBusMessage msg =
        QDBusMessage::createMethodCall(QLatin1String(s_kwinService), QLatin1String(s_kwinPath), QLatin1String(s_kwinPropertiesIface), QStringLiteral("Get"));
    msg << QLatin1String(s_kwinIface) << QStringLiteral("mode");
    const QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
        return reply.arguments().first().value<QDBusVariant>().variant().toInt();
    }
    return -1;
}

void setKwinMode(int mode)
{
    QDBusMessage msg =
        QDBusMessage::createMethodCall(QLatin1String(s_kwinService), QLatin1String(s_kwinPath), QLatin1String(s_kwinPropertiesIface), QStringLiteral("Set"));
    msg << QLatin1String(s_kwinIface) << QStringLiteral("mode") << QVariant::fromValue(QDBusVariant(QVariant::fromValue(mode)));
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

bool kwinVisible()
{
    QDBusMessage msg =
        QDBusMessage::createMethodCall(QLatin1String(s_kwinService), QLatin1String(s_kwinPath), QLatin1String(s_kwinPropertiesIface), QStringLiteral("Get"));
    msg << QLatin1String(s_kwinIface) << QStringLiteral("visible");
    const QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
        return reply.arguments().first().value<QDBusVariant>().variant().toBool();
    }
    return false;
}

void activateKwinKeyboard()
{
    QDBusMessage msg =
        QDBusMessage::createMethodCall(QLatin1String(s_kwinService), QLatin1String(s_kwinPath), QLatin1String(s_kwinIface), QStringLiteral("forceActivate"));
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

// Runs JavaScript inside plasmashell (used to control the Plasma panels).
void evaluatePlasmaScript(const QString &script, bool blocking = false)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.plasmashell"),
                                                      QStringLiteral("/PlasmaShell"),
                                                      QStringLiteral("org.kde.PlasmaShell"),
                                                      QStringLiteral("evaluateScript"));
    msg << script;
    if (blocking) {
        QDBusConnection::sessionBus().call(msg, QDBus::Block, 1000);
    } else {
        QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
    }
}

// Panel containment ids mapped to their configured hiding mode, read from the
// Plasma config so the user's setting can be restored after hiding the panel.
QHash<int, QString> configuredPanelHidingModes()
{
    QHash<int, QString> modes;
    KConfig config(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/plasma-org.kde.plasma.desktop-appletsrc"),
                   KConfig::SimpleConfig);
    const KConfigGroup containments = config.group(QStringLiteral("Containments"));
    const QStringList ids = containments.groupList();
    for (const QString &id : ids) {
        const KConfigGroup containment = containments.group(id);
        if (containment.readEntry(QStringLiteral("plugin")) != QLatin1String("org.kde.panel")) {
            continue;
        }
        bool ok = false;
        const int panelId = id.toInt(&ok);
        if (ok) {
            modes.insert(panelId, containment.readEntry(QStringLiteral("hiding"), QStringLiteral("none")));
        }
    }
    return modes;
}
} // namespace

/**
 * Shows the keyboard when the global shortcut is pressed. KWin only shows the
 * panel for the configured input mode, so temporarily switch to AnyInput and
 * restore the previous mode once the panel is hidden again.
 */
class KeyboardHotkeyController : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardHotkeyController(QObject *parent = nullptr)
        : QObject(parent)
    {
        m_panelHidingModes = configuredPanelHidingModes();

        QDBusConnection::sessionBus().connect(QLatin1String(s_kwinService),
                                              QLatin1String(s_kwinPath),
                                              QLatin1String(s_kwinIface),
                                              QStringLiteral("visibleChanged"),
                                              this,
                                              SLOT(restoreModeIfHidden()));
        QDBusConnection::sessionBus().connect(QLatin1String(s_kwinService),
                                              QLatin1String(s_kwinPath),
                                              QLatin1String(s_kwinIface),
                                              QStringLiteral("visibleChanged"),
                                              this,
                                              SLOT(updatePanelVisibility()));
        QDBusConnection::sessionBus().connect(QLatin1String(s_kwinService),
                                              QLatin1String(s_kwinPath),
                                              QLatin1String(s_kwinPropertiesIface),
                                              QStringLiteral("PropertiesChanged"),
                                              this,
                                              SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

        // The PropertiesChanged signal is not always delivered; poll instead.
        auto *pollTimer = new QTimer(this);
        pollTimer->setInterval(1000);
        connect(pollTimer, &QTimer::timeout, this, &KeyboardHotkeyController::restoreModeIfHidden);
        connect(pollTimer, &QTimer::timeout, this, &KeyboardHotkeyController::updatePanelVisibility);
        pollTimer->start();

        // Apply the configured input mode on startup and when it changes.
        m_settingsWatcher = KConfigWatcher::create(PlasmaKeyboardSettings::self()->sharedConfig());
        connect(m_settingsWatcher.get(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &, const QByteArrayList &) {
            PlasmaKeyboardSettings::self()->load();
            applyConfiguredMode();
            updatePanelVisibility();
        });
        applyConfiguredMode();
        updatePanelVisibility();

        // Never leave the panel hidden behind us if we are killed.
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
            restorePanels(true);
        });
    }

public Q_SLOTS:
    void showKeyboard()
    {
        qCDebug(PlasmaKeyboard) << "Show-virtual-keyboard shortcut triggered";
        if (kwinVisible()) {
            // Toggle: hide the keyboard if it is currently shown.
            QGuiApplication::inputMethod()->hide();
            return;
        }
        // AnyInput so the panel is shown regardless of the last input device.
        setKwinMode(2);
        activateKwinKeyboard();
    }

    // Restore the configured mode once the panel is hidden.
    void restoreModeIfHidden()
    {
        if (!kwinVisible()) {
            applyConfiguredMode();
        }
    }

    // Hide the Plasma panel(s) while the keyboard is visible, so the keyboard
    // reaches the bottom of the screen, and restore them afterwards.
    void updatePanelVisibility()
    {
        if (PlasmaKeyboardSettings::self()->hidePanelWhenKeyboardVisible() && kwinVisible()) {
            hidePanels();
        } else {
            restorePanels();
        }
    }

private Q_SLOTS:
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changed, const QStringList &invalidated)
    {
        Q_UNUSED(invalidated);
        if (interfaceName != QLatin1String(s_kwinIface)) {
            return;
        }
        if (changed.contains(QStringLiteral("visible")) && !changed.value(QStringLiteral("visible")).toBool()) {
            applyConfiguredMode();
        }
    }

private:
    // NonMouseInput (1) unless the user wants the keyboard also on mouse focus.
    static int configuredMode()
    {
        return PlasmaKeyboardSettings::self()->showOnMouseFocus() ? 2 : 1;
    }

    void applyConfiguredMode()
    {
        if (kwinVisible()) {
            return;
        }
        if (kwinMode() != configuredMode()) {
            setKwinMode(configuredMode());
        }
    }

    void hidePanels()
    {
        if (m_panelsHidden) {
            return;
        }
        m_panelsHidden = true;
        evaluatePlasmaScript(QStringLiteral("panelIds.forEach(function(id){var p=panelById(id);if(p.hiding!==\"autohide\"){p.hiding=\"autohide\";}});"));
    }

    void restorePanels(bool blocking = false)
    {
        if (!m_panelsHidden) {
            return;
        }
        m_panelsHidden = false;
        QJsonObject modes;
        for (auto it = m_panelHidingModes.constBegin(); it != m_panelHidingModes.constEnd(); ++it) {
            modes.insert(QString::number(it.key()), it.value());
        }
        const QString modesJson = QString::fromUtf8(QJsonDocument(modes).toJson(QJsonDocument::Compact));
        evaluatePlasmaScript(
            QStringLiteral("var m=%1;panelIds.forEach(function(id){var p=panelById(id);p.hiding=(m[id]!==undefined?m[id]:\"none\");});").arg(modesJson),
            blocking);
    }

    KConfigWatcher::Ptr m_settingsWatcher;
    QHash<int, QString> m_panelHidingModes;
    bool m_panelsHidden = false;
};

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
    auto *hotkeyController = new KeyboardHotkeyController(&application);
    QObject::connect(showAction, &QAction::triggered, hotkeyController, &KeyboardHotkeyController::showKeyboard);

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

#include "main.moc"
