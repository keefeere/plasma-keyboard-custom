/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "clipboardhistory.h"

#include "logging.h"
#include "plasmakeyboardsettings.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>

namespace
{
//! Connection name for the read-only access to the clipboard database.
const QString s_connectionName = QStringLiteral("plasma-keyboard-clipboard");

//! Schema version of the history database this knows how to read.
constexpr int s_knownDatabaseVersion = 3;

//! How often the database file is checked for changes, in milliseconds.
constexpr int s_pollIntervalMs = 1500;
}

ClipboardHistory::ClipboardHistory(QObject *parent)
    : QAbstractListModel(parent)
{
    m_databaseFile = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/klipper/history3.sqlite"));

    m_pollTimer.setInterval(s_pollIntervalMs);
    connect(&m_pollTimer, &QTimer::timeout, this, [this] {
        const QDateTime modified = lastModified();
        if (modified.isValid() && modified != m_lastModified) {
            refresh();
        }
    });

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

void ClipboardHistory::updateEnabled()
{
    if (!PlasmaKeyboardSettings::self()->clipboardEnabled()) {
        m_pollTimer.stop();
        m_lastModified = QDateTime();
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

QDateTime ClipboardHistory::lastModified() const
{
    QDateTime newest = m_databaseFile.lastModified();
    for (const QString &suffix : {QStringLiteral("-wal"), QStringLiteral("-shm")}) {
        const QDateTime modified = QFileInfo(m_databaseFile.absoluteFilePath() + suffix).lastModified();
        if (modified > newest) {
            newest = modified;
        }
    }
    return newest;
}

void ClipboardHistory::refresh()
{
    QStringList entries;

    {
        QSqlDatabase database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), s_connectionName);
        database.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
        database.setDatabaseName(m_databaseFile.absoluteFilePath());
        if (database.open()) {
            QSqlQuery query(database);
            const bool knownVersion =
                query.exec(QStringLiteral("SELECT db_version FROM version")) && query.next() && query.value(0).toInt() == s_knownDatabaseVersion;
            // Same order as the Plasma clipboard applet, so both show the same
            // most recently used entry first.
            if (knownVersion
                && query.exec(QStringLiteral("SELECT text FROM main WHERE text IS NOT NULL AND text <> '' ORDER BY last_used_time DESC, added_time DESC"))) {
                while (query.next()) {
                    const QString text = query.value(0).toString();
                    if (!text.isEmpty()) {
                        entries.append(text);
                    }
                }
            }
            database.close();
        }
    }
    QSqlDatabase::removeDatabase(s_connectionName);

    m_lastModified = lastModified();
    qCDebug(PlasmaKeyboard) << "clipboard history entries:" << entries.count() << "from" << m_databaseFile.absoluteFilePath();

    if (entries == m_entries) {
        return;
    }

    beginResetModel();
    m_entries = entries;
    endResetModel();
    Q_EMIT countChanged();
}
