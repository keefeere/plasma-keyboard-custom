/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"
#include "../src/layoutpathhelper.h"
#include "../src/theme/thememanager.h"

#include <QVariantMap>
#include <qqml.h>

K_PLUGIN_CLASS_WITH_JSON(PlasmaKeyboardKcm, "kcm_plasmakeyboardcustom.json")

PlasmaKeyboardKcm::PlasmaKeyboardKcm(QObject *parent, const KPluginMetaData &metaData)
    : KQuickManagedConfigModule(parent, metaData)
{
    initLayoutsPath();

    // The KCM is a separate process without the keyboard's QML theme layer, so
    // exports fall back to the base palette plus the stored overrides.
    ThemeManager::instance()->setQmlEngine(nullptr);

    // clang-format off
    qmlRegisterSingletonInstance<PlasmaKeyboardSettings>(
        "org.kde.plasma.keyboard.custom.settings",
        1,
        0,
        "PlasmaKeyboardSettings",
        PlasmaKeyboardSettings::self()
    );
    // clang-format on

    load();
}

bool PlasmaKeyboardKcm::soundEnabled() const
{
    return m_soundEnabled;
}

void PlasmaKeyboardKcm::setSoundEnabled(bool soundEnabled)
{
    if (soundEnabled == m_soundEnabled) {
        return;
    }

    m_soundEnabled = soundEnabled;
    Q_EMIT soundEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::vibrationEnabled() const
{
    return m_vibrationEnabled;
}

void PlasmaKeyboardKcm::setVibrationEnabled(bool vibrationEnabled)
{
    if (vibrationEnabled == m_vibrationEnabled) {
        return;
    }

    m_vibrationEnabled = vibrationEnabled;
    Q_EMIT vibrationEnabledChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::vibrationStrength() const
{
    return m_vibrationStrength;
}

void PlasmaKeyboardKcm::setVibrationStrength(int vibrationStrength)
{
    if (vibrationStrength == m_vibrationStrength) {
        return;
    }

    m_vibrationStrength = vibrationStrength;
    Q_EMIT vibrationStrengthChanged();

    setNeedsSave(true);
}

QStringList PlasmaKeyboardKcm::enabledLocales() const
{
    return m_enabledLocales;
}

void PlasmaKeyboardKcm::enableLocale(const QString &locale)
{
    if (m_enabledLocales.contains(locale)) {
        return;
    }

    m_enabledLocales.append(locale);
    Q_EMIT enabledLocalesChanged();

    setNeedsSave(true);
}

void PlasmaKeyboardKcm::disableLocale(const QString &locale)
{
    if (!m_enabledLocales.contains(locale)) {
        return;
    }

    m_enabledLocales.removeAll(locale);
    Q_EMIT enabledLocalesChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::keyboardNavigationEnabled() const
{
    return m_keyboardNavigationEnabled;
}

void PlasmaKeyboardKcm::setKeyboardNavigationEnabled(bool keyboardNavigationEnabled)
{
    if (keyboardNavigationEnabled == m_keyboardNavigationEnabled) {
        return;
    }

    m_keyboardNavigationEnabled = keyboardNavigationEnabled;
    Q_EMIT keyboardNavigationEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::autoCapitalizationEnabled() const
{
    return m_autoCapitalizationEnabled;
}

void PlasmaKeyboardKcm::setAutoCapitalizationEnabled(bool autoCapitalizationEnabled)
{
    if (autoCapitalizationEnabled == m_autoCapitalizationEnabled) {
        return;
    }

    m_autoCapitalizationEnabled = autoCapitalizationEnabled;
    Q_EMIT autoCapitalizationEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::showOnMouseFocus() const
{
    return m_showOnMouseFocus;
}

void PlasmaKeyboardKcm::setShowOnMouseFocus(bool showOnMouseFocus)
{
    if (showOnMouseFocus == m_showOnMouseFocus) {
        return;
    }

    m_showOnMouseFocus = showOnMouseFocus;
    Q_EMIT showOnMouseFocusChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::showOnLongTap() const
{
    return m_showOnLongTap;
}

void PlasmaKeyboardKcm::setShowOnLongTap(bool showOnLongTap)
{
    if (showOnLongTap == m_showOnLongTap) {
        return;
    }

    m_showOnLongTap = showOnLongTap;
    Q_EMIT showOnLongTapChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::showFunctionKeyRow() const
{
    return m_showFunctionKeyRow;
}

void PlasmaKeyboardKcm::setShowFunctionKeyRow(bool showFunctionKeyRow)
{
    if (showFunctionKeyRow == m_showFunctionKeyRow) {
        return;
    }

    m_showFunctionKeyRow = showFunctionKeyRow;
    Q_EMIT showFunctionKeyRowChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::showOnLongTapThresholdMs() const
{
    return m_showOnLongTapThresholdMs;
}

void PlasmaKeyboardKcm::setShowOnLongTapThresholdMs(int showOnLongTapThresholdMs)
{
    if (showOnLongTapThresholdMs == m_showOnLongTapThresholdMs) {
        return;
    }

    m_showOnLongTapThresholdMs = showOnLongTapThresholdMs;
    Q_EMIT showOnLongTapThresholdMsChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::hidePanelWhenKeyboardVisible() const
{
    return m_hidePanelWhenKeyboardVisible;
}

void PlasmaKeyboardKcm::setHidePanelWhenKeyboardVisible(bool hide)
{
    if (hide == m_hidePanelWhenKeyboardVisible) {
        return;
    }

    m_hidePanelWhenKeyboardVisible = hide;
    Q_EMIT hidePanelWhenKeyboardVisibleChanged();

    setNeedsSave(true);
}

QString PlasmaKeyboardKcm::keyboardFontFamily() const
{
    return m_keyboardFontFamily;
}

void PlasmaKeyboardKcm::setKeyboardFontFamily(const QString &family)
{
    if (family == m_keyboardFontFamily) {
        return;
    }

    m_keyboardFontFamily = family;
    Q_EMIT keyboardFontFamilyChanged();

    setNeedsSave(true);
}

QString PlasmaKeyboardKcm::theme() const
{
    return m_theme;
}

void PlasmaKeyboardKcm::setTheme(const QString &theme)
{
    if (theme == m_theme) {
        return;
    }

    m_theme = theme;
    Q_EMIT themeChanged();

    setNeedsSave(true);
}

QVariantList PlasmaKeyboardKcm::availableThemes() const
{
    return ThemeManager::instance()->availableThemes();
}

QString PlasmaKeyboardKcm::installTheme(const QUrl &source)
{
    const QString error = ThemeManager::instance()->installTheme(source);
    if (error.isEmpty()) {
        Q_EMIT availableThemesChanged();
    }
    return error;
}

QString PlasmaKeyboardKcm::exportTheme(const QString &id, const QUrl &target)
{
    return ThemeManager::instance()->exportTheme(id, target);
}

QString PlasmaKeyboardKcm::removeUserTheme(const QString &id)
{
    const QString error = ThemeManager::instance()->removeUserTheme(id);
    if (error.isEmpty()) {
        Q_EMIT availableThemesChanged();
        if (m_theme == id) {
            setTheme(QStringLiteral("system"));
        }
    }
    return error;
}

int PlasmaKeyboardKcm::keyboardHeightPercent() const
{
    return m_keyboardHeightPercent;
}

void PlasmaKeyboardKcm::setKeyboardHeightPercent(int percent)
{
    if (percent == m_keyboardHeightPercent) {
        return;
    }

    m_keyboardHeightPercent = percent;
    Q_EMIT keyboardHeightPercentChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::clipboardEnabled() const
{
    return m_clipboardEnabled;
}

void PlasmaKeyboardKcm::setClipboardEnabled(bool clipboardEnabled)
{
    if (clipboardEnabled == m_clipboardEnabled) {
        return;
    }

    m_clipboardEnabled = clipboardEnabled;
    setNeedsSave(true);
    Q_EMIT clipboardEnabledChanged();
}

bool PlasmaKeyboardKcm::diacriticsPopupEnabled() const
{
    return m_diacriticsPopupEnabled;
}

void PlasmaKeyboardKcm::setDiacriticsPopupEnabled(bool enabled)
{
    if (enabled == m_diacriticsPopupEnabled) {
        return;
    }

    m_diacriticsPopupEnabled = enabled;
    Q_EMIT diacriticsPopupEnabledChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::diacriticsHoldThresholdMs() const
{
    return m_diacriticsHoldThresholdMs;
}

void PlasmaKeyboardKcm::setDiacriticsHoldThresholdMs(int thresholdMs)
{
    if (thresholdMs == m_diacriticsHoldThresholdMs) {
        return;
    }

    m_diacriticsHoldThresholdMs = thresholdMs;
    Q_EMIT diacriticsHoldThresholdMsChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::isSaveNeeded() const
{
    return m_saveNeeded;
}

void PlasmaKeyboardKcm::load()
{
    setSoundEnabled(PlasmaKeyboardSettings::self()->soundEnabled());
    setVibrationEnabled(PlasmaKeyboardSettings::self()->vibrationEnabled());
    setVibrationStrength(PlasmaKeyboardSettings::self()->vibrationStrength());

    m_enabledLocales = PlasmaKeyboardSettings::self()->enabledLocales();
    Q_EMIT enabledLocalesChanged();
    setKeyboardNavigationEnabled(PlasmaKeyboardSettings::self()->keyboardNavigationEnabled());
    setAutoCapitalizationEnabled(PlasmaKeyboardSettings::self()->autoCapitalizationEnabled());
    setShowOnMouseFocus(PlasmaKeyboardSettings::self()->showOnMouseFocus());
    setShowOnLongTap(PlasmaKeyboardSettings::self()->showOnLongTap());
    setShowFunctionKeyRow(PlasmaKeyboardSettings::self()->showFunctionKeyRow());
    setClipboardEnabled(PlasmaKeyboardSettings::self()->clipboardEnabled());
    setShowOnLongTapThresholdMs(PlasmaKeyboardSettings::self()->showOnLongTapThresholdMs());
    setHidePanelWhenKeyboardVisible(PlasmaKeyboardSettings::self()->hidePanelWhenKeyboardVisible());
    setKeyboardFontFamily(PlasmaKeyboardSettings::self()->keyboardFontFamily());
    setTheme(PlasmaKeyboardSettings::self()->theme());
    setKeyboardHeightPercent(PlasmaKeyboardSettings::self()->keyboardHeightPercent());
    setDiacriticsPopupEnabled(PlasmaKeyboardSettings::self()->diacriticsPopupEnabled());
    setDiacriticsHoldThresholdMs(PlasmaKeyboardSettings::self()->diacriticsHoldThresholdMs());

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::save()
{
    PlasmaKeyboardSettings::self()->setSoundEnabled(m_soundEnabled);
    PlasmaKeyboardSettings::self()->setVibrationEnabled(m_vibrationEnabled);
    PlasmaKeyboardSettings::self()->setVibrationStrength(m_vibrationStrength);
    PlasmaKeyboardSettings::self()->setEnabledLocales(m_enabledLocales);
    PlasmaKeyboardSettings::self()->setKeyboardNavigationEnabled(m_keyboardNavigationEnabled);
    PlasmaKeyboardSettings::self()->setAutoCapitalizationEnabled(m_autoCapitalizationEnabled);
    PlasmaKeyboardSettings::self()->setShowOnMouseFocus(m_showOnMouseFocus);
    PlasmaKeyboardSettings::self()->setShowOnLongTap(m_showOnLongTap);
    PlasmaKeyboardSettings::self()->setShowFunctionKeyRow(m_showFunctionKeyRow);
    PlasmaKeyboardSettings::self()->setClipboardEnabled(m_clipboardEnabled);
    PlasmaKeyboardSettings::self()->setShowOnLongTapThresholdMs(m_showOnLongTapThresholdMs);
    PlasmaKeyboardSettings::self()->setHidePanelWhenKeyboardVisible(m_hidePanelWhenKeyboardVisible);
    PlasmaKeyboardSettings::self()->setKeyboardFontFamily(m_keyboardFontFamily);
    PlasmaKeyboardSettings::self()->setTheme(m_theme);
    PlasmaKeyboardSettings::self()->setKeyboardHeightPercent(m_keyboardHeightPercent);
    PlasmaKeyboardSettings::self()->setDiacriticsPopupEnabled(m_diacriticsPopupEnabled);
    PlasmaKeyboardSettings::self()->setDiacriticsHoldThresholdMs(m_diacriticsHoldThresholdMs);
    PlasmaKeyboardSettings::self()->save();

    setNeedsSave(false);
}

#include "plasmakeyboardkcm.moc"

#include "moc_plasmakeyboardkcm.cpp"
