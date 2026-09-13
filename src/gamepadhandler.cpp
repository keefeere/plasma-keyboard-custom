/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "gamepadhandler.h"
#include "logging.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QTimer>
#include <Qt>

namespace
{
constexpr auto s_service = "org.shadowblip.InputPlumber";
constexpr auto s_interface = "org.shadowblip.Input.DBusDevice";
constexpr auto s_signal = "InputEvent";

QString propertyStringList(const QString &path, const QString &interface, const QString &property, const QString &signature)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QLatin1String(s_service), path, QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    msg << interface << property;
    const QDBusMessage reply = QDBusConnection::systemBus().call(msg);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        return {};
    }
    const QVariant value = reply.arguments().first().value<QDBusVariant>().variant();
    if (signature == QLatin1String("as")) {
        return value.toStringList().value(0);
    }
    return {};
}

uint propertyUint(const QString &path, const QString &interface, const QString &property)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QLatin1String(s_service), path, QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    msg << interface << property;
    const QDBusMessage reply = QDBusConnection::systemBus().call(msg);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        return 0;
    }
    return reply.arguments().first().value<QDBusVariant>().variant().toUInt();
}

// Find the active composite device and the object path of its "dbus" target.
QString findDbusDevicePath(QString *compositePathOut)
{
    const QString managerPath = QStringLiteral("/org/shadowblip/InputPlumber/Manager");
    QString compositePath =
        propertyStringList(managerPath, QStringLiteral("org.shadowblip.InputManager"), QStringLiteral("GamepadOrder"), QStringLiteral("as"));
    if (compositePath.isEmpty()) {
        compositePath = QStringLiteral("/org/shadowblip/InputPlumber/CompositeDevice0");
    }

    if (compositePathOut) {
        *compositePathOut = compositePath;
    }

    return propertyStringList(compositePath, QStringLiteral("org.shadowblip.Input.CompositeDevice"), QStringLiteral("DbusDevices"), QStringLiteral("as"));
}
} // namespace

GamepadHandler::GamepadHandler(QObject *parent)
    : QObject(parent)
{
    m_repeatTimer = new QTimer(this);
    m_repeatTimer->setInterval(180);
    connect(m_repeatTimer, &QTimer::timeout, this, [this] {
        if (m_repeatKey != 0) {
            Q_EMIT navigate(m_repeatKey);
        }
    });

    const QString path = findDbusDevicePath(&m_compositePath);
    qCDebug(PlasmaKeyboard) << "GamepadHandler: InputPlumber dbus target" << (path.isEmpty() ? QStringLiteral("<not found>") : path);
    if (path.isEmpty()) {
        return;
    }

    const bool connected = QDBusConnection::systemBus().connect(QLatin1String(s_service),
                                                                path,
                                                                QLatin1String(s_interface),
                                                                QLatin1String(s_signal),
                                                                this,
                                                                SLOT(onInputEvent(QString, double)));
    if (connected) {
        m_available = true;
        Q_EMIT availableChanged();
    }

    // Interception follows the actual state of the panel as reported by KWin,
    // which is more reliable than the window visibility of this process.
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.KWin"),
                                          QStringLiteral("/VirtualKeyboard"),
                                          QStringLiteral("org.kde.kwin.VirtualKeyboard"),
                                          QStringLiteral("visibleChanged"),
                                          this,
                                          SLOT(refreshFromKWin()));
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.KWin"),
                                          QStringLiteral("/VirtualKeyboard"),
                                          QStringLiteral("org.freedesktop.DBus.Properties"),
                                          QStringLiteral("PropertiesChanged"),
                                          this,
                                          SLOT(onKWinPropertiesChanged(QString, QVariantMap, QStringList)));
    refreshFromKWin();

    // Safety net in case the PropertiesChanged signal is not delivered.
    m_kwinPollTimer = new QTimer(this);
    m_kwinPollTimer->setInterval(1000);
    connect(m_kwinPollTimer, &QTimer::timeout, this, &GamepadHandler::refreshFromKWin);
    m_kwinPollTimer->start();
}

void GamepadHandler::onKWinPropertiesChanged(const QString &interfaceName, const QVariantMap &changed, const QStringList &invalidated)
{
    Q_UNUSED(invalidated);
    if (interfaceName != QLatin1String("org.kde.kwin.VirtualKeyboard")) {
        return;
    }
    if (changed.contains(QStringLiteral("visible"))) {
        setActive(changed.value(QStringLiteral("visible")).toBool());
    }
}

void GamepadHandler::refreshFromKWin()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                      QStringLiteral("/VirtualKeyboard"),
                                                      QStringLiteral("org.freedesktop.DBus.Properties"),
                                                      QStringLiteral("Get"));
    msg << QStringLiteral("org.kde.kwin.VirtualKeyboard") << QStringLiteral("visible");
    const QDBusMessage reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        return;
    }
    setActive(reply.arguments().first().value<QDBusVariant>().variant().toBool());
}

GamepadHandler::~GamepadHandler()
{
    if (m_active) {
        setInterceptMode(m_savedInterceptMode);
    }
}

bool GamepadHandler::isAvailable() const
{
    return m_available;
}

bool GamepadHandler::isActive() const
{
    return m_active;
}

uint GamepadHandler::interceptMode() const
{
    if (m_compositePath.isEmpty()) {
        return 0;
    }
    return propertyUint(m_compositePath, QStringLiteral("org.shadowblip.Input.CompositeDevice"), QStringLiteral("InterceptMode"));
}

void GamepadHandler::setInterceptMode(uint mode)
{
    if (m_compositePath.isEmpty()) {
        return;
    }
    QDBusMessage msg =
        QDBusMessage::createMethodCall(QLatin1String(s_service), m_compositePath, QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Set"));
    msg << QStringLiteral("org.shadowblip.Input.CompositeDevice") << QStringLiteral("InterceptMode")
        << QVariant::fromValue(QDBusVariant(QVariant::fromValue(mode)));
    QDBusConnection::systemBus().call(msg);
}

void GamepadHandler::setActive(bool active)
{
    if (!m_available || m_compositePath.isEmpty()) {
        return;
    }
    if (active == m_active) {
        return;
    }

    if (active) {
        m_savedInterceptMode = interceptMode();
        // 3 = GAMEPAD_ONLY: gamepad input is routed over D-Bus and no longer
        // reaches the game (or Steam's mapping).
        setInterceptMode(3);
    } else {
        setInterceptMode(m_savedInterceptMode);
    }
    m_active = active;
    qCDebug(PlasmaKeyboard) << "GamepadHandler::setActive" << active << "saved mode" << m_savedInterceptMode << "now" << interceptMode();
}

void GamepadHandler::onInputEvent(const QString &event, double value)
{
    qCDebug(PlasmaKeyboard) << "GamepadHandler: InputEvent" << event << value;
    const bool pressed = value >= 0.5;

    // InputPlumber's dbus target emits abstract UI actions, not capability
    // names (see src/input/event/dbus.rs in InputPlumber).
    if (event == QLatin1String("ui_up")) {
        handleDirection(Qt::Key_Up, pressed);
        return;
    }
    if (event == QLatin1String("ui_down")) {
        handleDirection(Qt::Key_Down, pressed);
        return;
    }
    if (event == QLatin1String("ui_left")) {
        handleDirection(Qt::Key_Left, pressed);
        return;
    }
    if (event == QLatin1String("ui_right")) {
        handleDirection(Qt::Key_Right, pressed);
        return;
    }

    // All remaining mappings trigger on press only.
    if (!pressed) {
        return;
    }

    if (event == QLatin1String("ui_accept")) {
        // A: type the highlighted key.
        Q_EMIT activate();
    } else if (event == QLatin1String("ui_back")) {
        // B: close the keyboard.
        Q_EMIT hideKeyboard();
    } else if (event == QLatin1String("ui_context")) {
        // X: backspace.
        Q_EMIT backspace();
    } else if (event == QLatin1String("ui_action")) {
        // Y: space.
        Q_EMIT space();
    } else if (event == QLatin1String("ui_l2")) {
        // LT: shift.
        Q_EMIT toggleShift();
    } else if (event == QLatin1String("ui_l1")) {
        // LB: symbols layer.
        Q_EMIT toggleSymbols();
    } else if (event == QLatin1String("ui_r1")) {
        // RB: switch layout.
        Q_EMIT switchLanguage();
    } else if (event == QLatin1String("ui_r2")) {
        // RT: Enter.
        Q_EMIT enter();
    } else if (event == QLatin1String("ui_option")) {
        // Start: close the keyboard.
        Q_EMIT hideKeyboard();
    }
}

void GamepadHandler::handleDirection(int key, bool pressed)
{
    if (pressed) {
        if (!m_pressedDirections.contains(key)) {
            m_pressedDirections.insert(key);
            Q_EMIT navigate(key);
        }
        m_repeatKey = key;
        m_repeatTimer->start();
    } else {
        m_pressedDirections.remove(key);
        if (m_repeatKey == key) {
            m_repeatKey = 0;
            m_repeatTimer->stop();
        }
    }
}
