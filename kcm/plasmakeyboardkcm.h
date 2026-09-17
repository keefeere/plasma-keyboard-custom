/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <KQuickManagedConfigModule>

#include <QUrl>
#include <QVariantList>

#include "plasmakeyboardsettings.h"

class PlasmaKeyboardKcm : public KQuickManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(bool vibrationEnabled READ vibrationEnabled WRITE setVibrationEnabled NOTIFY vibrationEnabledChanged)
    Q_PROPERTY(int vibrationStrength READ vibrationStrength WRITE setVibrationStrength NOTIFY vibrationStrengthChanged)
    Q_PROPERTY(QStringList enabledLocales READ enabledLocales NOTIFY enabledLocalesChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ keyboardNavigationEnabled WRITE setKeyboardNavigationEnabled NOTIFY keyboardNavigationEnabledChanged)
    Q_PROPERTY(bool autoCapitalizationEnabled READ autoCapitalizationEnabled WRITE setAutoCapitalizationEnabled NOTIFY autoCapitalizationEnabledChanged)
    Q_PROPERTY(bool showOnMouseFocus READ showOnMouseFocus WRITE setShowOnMouseFocus NOTIFY showOnMouseFocusChanged)
    Q_PROPERTY(bool showOnLongTap READ showOnLongTap WRITE setShowOnLongTap NOTIFY showOnLongTapChanged)
    Q_PROPERTY(bool showFunctionKeyRow READ showFunctionKeyRow WRITE setShowFunctionKeyRow NOTIFY showFunctionKeyRowChanged)
    Q_PROPERTY(bool clipboardEnabled READ clipboardEnabled WRITE setClipboardEnabled NOTIFY clipboardEnabledChanged)
    Q_PROPERTY(int showOnLongTapThresholdMs READ showOnLongTapThresholdMs WRITE setShowOnLongTapThresholdMs NOTIFY showOnLongTapThresholdMsChanged)
    Q_PROPERTY(
        bool hidePanelWhenKeyboardVisible READ hidePanelWhenKeyboardVisible WRITE setHidePanelWhenKeyboardVisible NOTIFY hidePanelWhenKeyboardVisibleChanged)
    Q_PROPERTY(QString keyboardFontFamily READ keyboardFontFamily WRITE setKeyboardFontFamily NOTIFY keyboardFontFamilyChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QVariantList availableThemes READ availableThemes NOTIFY availableThemesChanged)
    Q_PROPERTY(int keyboardHeightPercent READ keyboardHeightPercent WRITE setKeyboardHeightPercent NOTIFY keyboardHeightPercentChanged)
    Q_PROPERTY(bool diacriticsPopupEnabled READ diacriticsPopupEnabled WRITE setDiacriticsPopupEnabled NOTIFY diacriticsPopupEnabledChanged)
    Q_PROPERTY(int diacriticsHoldThresholdMs READ diacriticsHoldThresholdMs WRITE setDiacriticsHoldThresholdMs NOTIFY diacriticsHoldThresholdMsChanged)

public:
    PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData);

    bool soundEnabled() const;
    void setSoundEnabled(bool soundEnabled);

    bool vibrationEnabled() const;
    void setVibrationEnabled(bool vibrationEnabled);

    int vibrationStrength() const;
    void setVibrationStrength(int vibrationStrength);

    QStringList enabledLocales() const;

    Q_INVOKABLE void enableLocale(const QString &locale);
    Q_INVOKABLE void disableLocale(const QString &locale);

    bool keyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool keyboardNavigationEnabled);

    bool autoCapitalizationEnabled() const;
    void setAutoCapitalizationEnabled(bool autoCapitalizationEnabled);

    bool showOnMouseFocus() const;
    void setShowOnMouseFocus(bool showOnMouseFocus);

    bool showOnLongTap() const;
    void setShowOnLongTap(bool showOnLongTap);

    bool showFunctionKeyRow() const;
    void setShowFunctionKeyRow(bool showFunctionKeyRow);

    bool clipboardEnabled() const;
    void setClipboardEnabled(bool clipboardEnabled);

    int showOnLongTapThresholdMs() const;
    void setShowOnLongTapThresholdMs(int showOnLongTapThresholdMs);

    bool hidePanelWhenKeyboardVisible() const;
    void setHidePanelWhenKeyboardVisible(bool hide);

    QString keyboardFontFamily() const;
    void setKeyboardFontFamily(const QString &family);

    QString theme() const;
    void setTheme(const QString &theme);

    QVariantList availableThemes() const;

    //! Import a theme file; an empty return means success.
    Q_INVOKABLE QString installTheme(const QUrl &source);

    //! Export a theme; an empty return means success.
    Q_INVOKABLE QString exportTheme(const QString &id, const QUrl &target);

    //! Remove a user theme; an empty return means success.
    Q_INVOKABLE QString removeUserTheme(const QString &id);

    int keyboardHeightPercent() const;
    void setKeyboardHeightPercent(int percent);

    bool diacriticsPopupEnabled() const;
    void setDiacriticsPopupEnabled(bool enabled);

    int diacriticsHoldThresholdMs() const;
    void setDiacriticsHoldThresholdMs(int thresholdMs);

    bool isSaveNeeded() const override;

public Q_SLOTS:
    void load() override;
    void save() override;

Q_SIGNALS:
    void soundEnabledChanged();
    void vibrationEnabledChanged();
    void vibrationStrengthChanged();
    void enabledLocalesChanged();
    void keyboardNavigationEnabledChanged();
    void autoCapitalizationEnabledChanged();
    void showOnMouseFocusChanged();
    void showOnLongTapChanged();
    void showFunctionKeyRowChanged();
    void clipboardEnabledChanged();
    void showOnLongTapThresholdMsChanged();
    void hidePanelWhenKeyboardVisibleChanged();
    void keyboardFontFamilyChanged();
    void themeChanged();
    void availableThemesChanged();
    void keyboardHeightPercentChanged();
    void diacriticsPopupEnabledChanged();
    void diacriticsHoldThresholdMsChanged();

private:
    bool m_soundEnabled = false;
    bool m_vibrationEnabled = true;
    int m_vibrationStrength = 75;
    bool m_keyboardNavigationEnabled = false;
    bool m_autoCapitalizationEnabled = true;
    bool m_showOnMouseFocus = false;
    bool m_showOnLongTap = false;
    bool m_showFunctionKeyRow = false;
    bool m_clipboardEnabled = false;
    int m_showOnLongTapThresholdMs = 600;
    bool m_hidePanelWhenKeyboardVisible = true;
    QString m_keyboardFontFamily;
    QString m_theme = QStringLiteral("system");
    int m_keyboardHeightPercent = 42;
    bool m_diacriticsPopupEnabled = true;
    int m_diacriticsHoldThresholdMs = 600;

    bool m_saveNeeded = false;

    QStringList m_enabledLocales;

    PlasmaKeyboardSettings *m_settings = nullptr;
};
