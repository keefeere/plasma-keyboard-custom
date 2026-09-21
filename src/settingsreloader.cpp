/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "settingsreloader.h"

#include "logging.h"
#include "plasmakeyboardsettings.h"

#include <QFileInfo>
#include <QStandardPaths>

namespace
{
//! Long enough to let KConfig finish writing the file, short enough to feel immediate.
constexpr int s_reloadDelayMs = 200;

//! The settings file as an absolute path: KConfig::name() returns the plain file name.
QString settingsFilePath()
{
    const QString name = PlasmaKeyboardSettings::self()->sharedConfig()->name();
    if (QFileInfo(name).isAbsolute()) {
        return name;
    }
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QLatin1Char('/') + name;
}
}

SettingsReloader::SettingsReloader(QObject *parent)
    : QObject(parent)
    , m_path(settingsFilePath())
    , m_lastModified(QFileInfo(m_path).lastModified())
{
    m_debounce.setSingleShot(true);
    m_debounce.setInterval(s_reloadDelayMs);
    connect(&m_debounce, &QTimer::timeout, this, &SettingsReloader::reload);

    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &SettingsReloader::scheduleReload);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, &SettingsReloader::scheduleReload);
    watchPath();

    qCDebug(PlasmaKeyboard) << "watching the keyboard settings file" << m_path << "watched:" << m_watcher.files() << m_watcher.directories();
}

void SettingsReloader::watchPath()
{
    const QString directory = QFileInfo(m_path).absolutePath();
    if (!directory.isEmpty() && !m_watcher.directories().contains(directory)) {
        m_watcher.addPath(directory);
    }
    if (QFileInfo::exists(m_path) && !m_watcher.files().contains(m_path)) {
        m_watcher.addPath(m_path);
    }
}

void SettingsReloader::scheduleReload()
{
    qCDebug(PlasmaKeyboard) << "keyboard settings file changed" << m_path;
    m_debounce.start();
}

void SettingsReloader::reload()
{
    watchPath();

    const QDateTime modified = QFileInfo(m_path).lastModified();
    if (modified == m_lastModified) {
        return;
    }
    m_lastModified = modified;

    PlasmaKeyboardSettings::self()->sharedConfig()->reparseConfiguration();
    PlasmaKeyboardSettings::self()->load();

    qCDebug(PlasmaKeyboard) << "keyboard settings re-read from" << m_path;
    Q_EMIT settingsReloaded();
}
