/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>
#include <QDateTime>
#include <QFileInfo>
#include <QStringList>
#include <QTimer>
#include <qqmlintegration.h>

/**
 * Recent clipboard entries, shown in a row above the keyboard.
 *
 * The keyboard window is never focused, so it cannot read the Wayland
 * selection itself. The entries are read read-only from the history database
 * of the Plasma clipboard manager instead (the same one the clipboard applet
 * fills), and the file is only touched while the feature is enabled in the
 * settings.
 */
class ClipboardHistory : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("ClipboardHistory is created in C++ and passed to QML.")

    //! Number of entries, so the row can hide itself when there are none.
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        TextRole = Qt::UserRole + 1,
    };

    explicit ClipboardHistory(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    //! Text of the entry at @p row, or an empty string when out of range.
    Q_INVOKABLE QString textAt(int row) const;

Q_SIGNALS:
    void countChanged();

private:
    //! Starts or stops following the database, depending on the setting.
    void updateEnabled();

    //! Re-reads the entries, if the database is there and readable.
    void refresh();

    /**
     * Newest modification time of the database and its write-ahead log files.
     *
     * The clipboard database runs in WAL mode, so changes usually only touch
     * the -wal file and leave the database itself untouched.
     */
    QDateTime lastModified() const;

    QTimer m_pollTimer;
    QFileInfo m_databaseFile;
    QStringList m_entries;
    QDateTime m_lastModified;
};
