/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QDateTime>
#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QTimer>

/*!
 * Re-reads the keyboard settings as soon as the configuration file changes.
 *
 * KConfigWatcher does not deliver changes in this application (verified
 * 2026-09-21), so the file and the directory it lives in are watched directly:
 * KConfig writes the file through a rename, which makes the watched path stop
 * being watched, so both paths are re-added after every event.
 *
 * The file time decides whether the settings are re-read, so that an unrelated
 * change in the configuration directory does not reload them.
 */
class SettingsReloader : public QObject
{
    Q_OBJECT

public:
    explicit SettingsReloader(QObject *parent = nullptr);

Q_SIGNALS:
    //! The settings were re-read; the values may have changed.
    void settingsReloaded();

private:
    //! Starts the debounce timer, so that a burst of file events causes one reload.
    void scheduleReload();
    //! Re-reads the settings, if the configuration file really changed.
    void reload();
    //! Keeps watching the file and its directory.
    void watchPath();

    QFileSystemWatcher m_watcher;
    QTimer m_debounce;
    QString m_path;
    QDateTime m_lastModified;
};
