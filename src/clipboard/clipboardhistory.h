/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <QTimer>
#include <qqmlintegration.h>

/**
 * Recent clipboard entries, shown in a row above the keyboard.
 *
 * The keyboard window is never focused, so it cannot read the Wayland
 * selection itself. The entries are asked for over D-Bus instead, from the
 * clipboard manager of the desktop (org.kde.klipper, i.e. the clipboard widget
 * in the Plasma panel), which keeps the history and tells us when it changes.
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

    //! Forgets the whole history (the clipboard manager does the same).
    Q_INVOKABLE void clear();

Q_SIGNALS:
    void countChanged();

private Q_SLOTS:
    //! Re-reads the entries from the clipboard manager.
    void refresh();

private:
    //! Starts or stops following the clipboard, depending on the setting.
    void updateEnabled();

    //! The entry at @p index, or an empty string when there is none.
    static QString historyItem(int index);

    QTimer m_pollTimer;
    QStringList m_entries;
};
