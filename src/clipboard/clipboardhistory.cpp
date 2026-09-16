/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "clipboardhistory.h"

#include "logging.h"
#include "plasmakeyboardsettings.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>

namespace
{
//! The clipboard manager of the desktop, as used by the Plasma clipboard widget.
constexpr auto s_service = "org.kde.klipper";
constexpr auto s_path = "/klipper";
constexpr auto s_interface = "org.kde.klipper.klipper";

//! Safety limit: the entries are fetched one by one until one comes back empty.
constexpr int s_maxEntries = 50;

//! How often the history is checked in case a signal got lost, in milliseconds.
constexpr int s_pollIntervalMs = 2000;

QVariant call(const QString &method, const QVariantList &arguments = {})
{
    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String(s_service), QLatin1String(s_path), QLatin1String(s_interface), method);
    if (!arguments.isEmpty()) {
        message.setArguments(arguments);
    }

    const QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        return {};
    }

    return reply.arguments().first();
}
}

ClipboardHistory::ClipboardHistory(QObject *parent)
    : QAbstractListModel(parent)
{
    // The clipboard widget tells us right away when the history changes.
    QDBusConnection::sessionBus()
        .connect(QLatin1String(s_service), QLatin1String(s_path), QLatin1String(s_interface), QStringLiteral("clipboardHistoryUpdated"), this, SLOT(refresh()));

    m_pollTimer.setInterval(s_pollIntervalMs);
    connect(&m_pollTimer, &QTimer::timeout, this, &ClipboardHistory::refresh);

    connect(PlasmaKeyboardSettings::self(), &PlasmaKeyboardSettings::clipboardEnabledChanged, this, &ClipboardHistory::updateEnabled);
    updateEnabled();
}

int ClipboardHistory::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_entries.count();
}

QVariant ClipboardHistory::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.count()) {
        return {};
    }

    switch (role) {
    case TextRole:
        return m_entries.at(index.row());
    default:
        return {};
    }
}

QHash<int, QByteArray> ClipboardHistory::roleNames() const
{
    return {{TextRole, QByteArrayLiteral("text")}};
}

int ClipboardHistory::count() const
{
    return m_entries.count();
}

QString ClipboardHistory::textAt(int row) const
{
    return m_entries.value(row);
}

QString ClipboardHistory::historyItem(int index)
{
    const QVariant value = call(QStringLiteral("getClipboardHistoryItem"), {index});

    // The entries are plain strings; a variant wrapper is unwrapped in case the
    // service decides to send one.
    if (value.metaType() == QMetaType::fromType<QDBusVariant>()) {
        return value.value<QDBusVariant>().variant().toString();
    }

    return value.toString();
}

void ClipboardHistory::clear()
{
    call(QStringLiteral("clearClipboardHistory"));

    // The clipboard manager reports the change as well, but empty the row right
    // away instead of waiting for that round trip.
    if (!m_entries.isEmpty()) {
        qCDebug(PlasmaKeyboard) << "clipboard history cleared";
        beginResetModel();
        m_entries.clear();
        endResetModel();
        Q_EMIT countChanged();
    }
}

void ClipboardHistory::updateEnabled()
{
    if (!PlasmaKeyboardSettings::self()->clipboardEnabled()) {
        m_pollTimer.stop();
        if (!m_entries.isEmpty()) {
            beginResetModel();
            m_entries.clear();
            endResetModel();
            Q_EMIT countChanged();
        }
        return;
    }

    refresh();
    m_pollTimer.start();
}

void ClipboardHistory::refresh()
{
    QStringList entries;
    for (int index = 0; index < s_maxEntries; ++index) {
        const QString entry = historyItem(index);
        if (entry.isEmpty()) {
            break;
        }
        entries.append(entry);
    }

    if (entries == m_entries) {
        return;
    }

    qCDebug(PlasmaKeyboard) << "clipboard history entries:" << entries.count();
    beginResetModel();
    m_entries = entries;
    endResetModel();
    Q_EMIT countChanged();
}
