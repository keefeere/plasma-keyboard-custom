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
    Q_PROPERTY(QString defaultLocale READ defaultLocale NOTIFY defaultLocaleChanged)
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
    Q_PROPERTY(
        int floatingKeyboardWidthPercent READ floatingKeyboardWidthPercent WRITE setFloatingKeyboardWidthPercent NOTIFY floatingKeyboardWidthPercentChanged)
    Q_PROPERTY(bool diacriticsPopupEnabled READ diacriticsPopupEnabled WRITE setDiacriticsPopupEnabled NOTIFY diacriticsPopupEnabledChanged)
    Q_PROPERTY(int diacriticsHoldThresholdMs READ diacriticsHoldThresholdMs WRITE setDiacriticsHoldThresholdMs NOTIFY diacriticsHoldThresholdMsChanged)
    Q_PROPERTY(bool gamepadAlternatesEnabled READ gamepadAlternatesEnabled WRITE setGamepadAlternatesEnabled NOTIFY gamepadAlternatesEnabledChanged)
    Q_PROPERTY(
        int gamepadAlternatesThresholdMs READ gamepadAlternatesThresholdMs WRITE setGamepadAlternatesThresholdMs NOTIFY gamepadAlternatesThresholdMsChanged)
    Q_PROPERTY(bool predictiveTextEnabled READ predictiveTextEnabled WRITE setPredictiveTextEnabled NOTIFY predictiveTextEnabledChanged)
    Q_PROPERTY(int predictiveSuggestionCount READ predictiveSuggestionCount WRITE setPredictiveSuggestionCount NOTIFY predictiveSuggestionCountChanged)
    Q_PROPERTY(int predictiveMinPrefixLength READ predictiveMinPrefixLength WRITE setPredictiveMinPrefixLength NOTIFY predictiveMinPrefixLengthChanged)
    Q_PROPERTY(bool predictiveNextWordEnabled READ predictiveNextWordEnabled WRITE setPredictiveNextWordEnabled NOTIFY predictiveNextWordEnabledChanged)
    Q_PROPERTY(bool predictiveTypoCorrectionEnabled READ predictiveTypoCorrectionEnabled WRITE setPredictiveTypoCorrectionEnabled NOTIFY
                   predictiveTypoCorrectionEnabledChanged)

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

    QString defaultLocale() const;

    //! Choose which of the enabled locales opens by default; empty lets the system locale decide.
    Q_INVOKABLE void setDefaultLocale(const QString &locale);

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

    int floatingKeyboardWidthPercent() const;
    void setFloatingKeyboardWidthPercent(int percent);

    bool diacriticsPopupEnabled() const;
    void setDiacriticsPopupEnabled(bool enabled);

    int diacriticsHoldThresholdMs() const;
    void setDiacriticsHoldThresholdMs(int thresholdMs);

    bool gamepadAlternatesEnabled() const;
    void setGamepadAlternatesEnabled(bool enabled);

    int gamepadAlternatesThresholdMs() const;
    void setGamepadAlternatesThresholdMs(int thresholdMs);

    bool predictiveTextEnabled() const;
    void setPredictiveTextEnabled(bool enabled);

    int predictiveSuggestionCount() const;
    void setPredictiveSuggestionCount(int count);

    int predictiveMinPrefixLength() const;
    void setPredictiveMinPrefixLength(int length);

    bool predictiveNextWordEnabled() const;
    void setPredictiveNextWordEnabled(bool enabled);

    bool predictiveTypoCorrectionEnabled() const;
    void setPredictiveTypoCorrectionEnabled(bool enabled);

    bool isSaveNeeded() const override;

public Q_SLOTS:
    void load() override;
    void save() override;

Q_SIGNALS:
    void soundEnabledChanged();
    void vibrationEnabledChanged();
    void vibrationStrengthChanged();
    void enabledLocalesChanged();
    void defaultLocaleChanged();
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
    void floatingKeyboardWidthPercentChanged();
    void diacriticsPopupEnabledChanged();
    void diacriticsHoldThresholdMsChanged();
    void gamepadAlternatesEnabledChanged();
    void gamepadAlternatesThresholdMsChanged();
    void predictiveTextEnabledChanged();
    void predictiveSuggestionCountChanged();
    void predictiveMinPrefixLengthChanged();
    void predictiveNextWordEnabledChanged();
    void predictiveTypoCorrectionEnabledChanged();

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
    int m_floatingKeyboardWidthPercent = 80;
    bool m_diacriticsPopupEnabled = true;
    int m_diacriticsHoldThresholdMs = 600;
    bool m_gamepadAlternatesEnabled = true;
    int m_gamepadAlternatesThresholdMs = 400;
    bool m_predictiveTextEnabled = true;
    int m_predictiveSuggestionCount = 3;
    int m_predictiveMinPrefixLength = 1;
    bool m_predictiveNextWordEnabled = true;
    bool m_predictiveTypoCorrectionEnabled = true;

    bool m_saveNeeded = false;

    QStringList m_enabledLocales;
    QString m_defaultLocale;

    PlasmaKeyboardSettings *m_settings = nullptr;
};
