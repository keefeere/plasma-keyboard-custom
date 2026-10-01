/*
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "focusededitabletracker.h"

#include "logging.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusVariant>
#include <QGuiApplication>
#include <QScreen>
#include <QUuid>

namespace
{
constexpr uint editableState = 7;
constexpr uint screenCoordinates = 0;
const QString focusedEvent = QStringLiteral("object:state-changed:focused");

bool hasState(const QList<uint> &words, uint state)
{
    const qsizetype word = state / 32;
    return word < words.size() && (words.at(word) & (uint(1) << (state % 32)));
}

QPointF mapTouchToScreen(const QPointF &position, const QScreen *screen)
{
    QPointF mapped = position;
    switch (screen->angleBetween(screen->nativeOrientation(), screen->orientation())) {
    case 90:
        mapped = QPointF(1.0 - position.y(), position.x());
        break;
    case 180:
        mapped = QPointF(1.0 - position.x(), 1.0 - position.y());
        break;
    case 270:
        mapped = QPointF(position.y(), 1.0 - position.x());
        break;
    default:
        break;
    }

    const QRect geometry = screen->geometry();
    return QPointF(geometry.x() + mapped.x() * geometry.width(), geometry.y() + mapped.y() * geometry.height());
}
}

FocusedEditableTracker::FocusedEditableTracker(QObject *parent)
    : QObject(parent)
{
    QDBusInterface accessibilityBus(QStringLiteral("org.a11y.Bus"),
                                    QStringLiteral("/org/a11y/bus"),
                                    QStringLiteral("org.a11y.Bus"),
                                    QDBusConnection::sessionBus());
    const QDBusReply<QString> address = accessibilityBus.call(QStringLiteral("GetAddress"));
    if (!address.isValid()) {
        return;
    }

    m_connectionName = QStringLiteral("plasma-keyboard-atspi-%1").arg(QUuid::createUuid().toString(QUuid::Id128));
    auto bus = QDBusConnection::connectToBus(address.value(), m_connectionName);
    if (!bus.isConnected()) {
        QDBusConnection::disconnectFromBus(m_connectionName);
        m_connectionName.clear();
        return;
    }
    m_bus = std::make_unique<QDBusConnection>(bus);

    QDBusInterface registry(QStringLiteral("org.a11y.atspi.Registry"),
                            QStringLiteral("/org/a11y/atspi/registry"),
                            QStringLiteral("org.a11y.atspi.Registry"),
                            *m_bus);
    const QDBusMessage registration = registry.call(QStringLiteral("RegisterEvent"),
                                                     focusedEvent,
                                                     QStringList(),
                                                     m_bus->baseService());
    if (registration.type() == QDBusMessage::ErrorMessage
        || !m_bus->connect(QString(),
                           QString(),
                           QStringLiteral("org.a11y.atspi.Event.Object"),
                           QStringLiteral("StateChanged"),
                           this,
                           SLOT(stateChanged(QString,int,int,QDBusVariant,QVariantMap,QDBusMessage)))) {
        m_bus.reset();
        QDBusConnection::disconnectFromBus(m_connectionName);
        m_connectionName.clear();
    }
}

FocusedEditableTracker::~FocusedEditableTracker()
{
    if (m_bus) {
        QDBusInterface registry(QStringLiteral("org.a11y.atspi.Registry"),
                                QStringLiteral("/org/a11y/atspi/registry"),
                                QStringLiteral("org.a11y.atspi.Registry"),
                                *m_bus);
        registry.call(QDBus::NoBlock,
                      QStringLiteral("DeregisterEvent"),
                      focusedEvent,
                      m_bus->baseService());
        m_bus.reset();
    }
    if (!m_connectionName.isEmpty()) {
        QDBusConnection::disconnectFromBus(m_connectionName);
    }
}

bool FocusedEditableTracker::containsNormalizedTouch(const QPointF &position) const
{
    const QScreen *screen = QGuiApplication::primaryScreen();
    return screen && m_focusedEditableRect.isValid() && m_focusedEditableRect.contains(mapTouchToScreen(position, screen).toPoint());
}

void FocusedEditableTracker::stateChanged(const QString &state,
                                          int enabled,
                                          int,
                                          const QDBusVariant &,
                                          const QVariantMap &,
                                          const QDBusMessage &message)
{
    if (state.compare(QStringLiteral("focused"), Qt::CaseInsensitive) != 0 || !m_bus) {
        return;
    }

    if (!enabled) {
        if (message.service() == m_focusedService && message.path() == m_focusedPath) {
            m_focusedService.clear();
            m_focusedPath.clear();
            m_focusedEditableRect = {};
        }
        return;
    }

    QDBusInterface accessible(message.service(),
                              message.path(),
                              QStringLiteral("org.a11y.atspi.Accessible"),
                              *m_bus);
    const QDBusMessage stateReply = accessible.call(QStringLiteral("GetState"));
    if (stateReply.type() == QDBusMessage::ErrorMessage || stateReply.arguments().isEmpty()) {
        return;
    }

    QList<uint> states;
    const QDBusArgument stateArgument = stateReply.arguments().constFirst().value<QDBusArgument>();
    stateArgument >> states;
    if (!hasState(states, editableState)) {
        return;
    }

    QDBusInterface component(message.service(),
                             message.path(),
                             QStringLiteral("org.a11y.atspi.Component"),
                             *m_bus);
    const QDBusMessage extentsReply = component.call(QStringLiteral("GetExtents"), screenCoordinates);
    if (extentsReply.type() == QDBusMessage::ErrorMessage || extentsReply.arguments().isEmpty()) {
        return;
    }

    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    const QDBusArgument extents = extentsReply.arguments().constFirst().value<QDBusArgument>();
    extents.beginStructure();
    extents >> x >> y >> width >> height;
    extents.endStructure();
    const QRect rectangle(x, y, width, height);
    if (!rectangle.isValid()) {
        return;
    }

    m_focusedService = message.service();
    m_focusedPath = message.path();
    m_focusedEditableRect = rectangle;
    qCDebug(PlasmaKeyboard) << "Focused editable AT-SPI rectangle:" << rectangle;
}
