/*
    SPDX-FileCopyrightText: 2026 Plasma Keyboard contributors

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QPointer>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

class QQmlEngine;

/**
 * User themes, stored as data (JSON) in the
 * ~/.local/share/plasma-keyboard/themes directory.
 *
 * The manager scans that directory, validates imported files against a
 * whitelist of keys and types (a theme is data, it is never executed) and
 * exposes the built-in themes so the KCM has a single list.
 *
 * An instance is registered for QML in main.cpp the same way as Modifiers.
 */
class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList availableThemes READ availableThemes NOTIFY themesChanged)

public:
    static ThemeManager *instance();

    //! Built-in themes plus the user themes found in the theme directory.
    //! Each entry is {id, name, source, base}, with source builtin|user.
    QVariantList availableThemes() const;

    //! Text of a local file, or an empty string when it cannot be read.
    Q_INVOKABLE QString readTextFile(const QUrl &url) const;

    //! Parsed content of a user theme (empty for built-ins or missing files).
    Q_INVOKABLE QVariantMap themeDescription(const QString &id) const;

    //! Validates @p source and copies it into the user directory. An empty
    //! return means success, otherwise a short user-readable error.
    Q_INVOKABLE QString installTheme(const QUrl &source, const QString &name = QString());

    //! Writes a full snapshot of @p id (base plus overrides) to @p target.
    Q_INVOKABLE QString exportTheme(const QString &id, const QUrl &target);

    //! Removes a user theme; an empty return means success.
    Q_INVOKABLE QString removeUserTheme(const QString &id);

    //! The engine used to read the effective palette from the QML theme layer.
    void setQmlEngine(QQmlEngine *engine);

Q_SIGNALS:
    void themesChanged();

private:
    explicit ThemeManager(QObject *parent = nullptr);

    struct ThemeData {
        QString name;
        QString base;
        QJsonObject palette;
        QJsonObject geometry;
        QJsonObject background;
        QJsonObject keyStyle;
        QJsonObject keyColors;
    };

    static QString themesDirectory();
    static bool isBuiltinId(const QString &id);
    static QString slugify(const QString &name);
    static QVariantList builtinThemes();

    //! Validates a parsed theme object and fills @p out with only the known
    //! sections. Returns false and sets @p error on the first offending key.
    static bool parseTheme(const QJsonObject &root, const QString &fallbackName, ThemeData &out, QString &error);
    static QJsonObject themeToJson(const ThemeData &theme);

    QObject *themeSingleton() const;
    bool currentThemeId(QString *id) const;
    bool readEffectiveTheme(ThemeData &out, QString &error) const;

    QPointer<QQmlEngine> m_engine;
};
