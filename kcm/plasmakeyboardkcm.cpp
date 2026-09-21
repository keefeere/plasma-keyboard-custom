/*
    SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
    SPDX-FileCopyrightText: 2026 Kristen McWilliam <kristen@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "plasmakeyboardkcm.h"
#include "../src/layoutpathhelper.h"
#include "../src/theme/thememanager.h"

#include <KGlobalAccel>
#include <KLocalizedString>

#include <QAction>
#include <QKeySequence>
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

    // The keyboard process registers this action in KGlobalAccel (see
    // src/main.cpp). Reusing its component and action names lets the KCM read
    // and change the global shortcut that shows the keyboard.
    m_showKeyboardAction = new QAction(this);
    m_showKeyboardAction->setObjectName(QStringLiteral("show-virtual-keyboard"));
    m_showKeyboardAction->setText(i18n("Show Virtual Keyboard"));
    m_showKeyboardAction->setProperty("componentName", QStringLiteral("org.kde.plasma.keyboard.custom"));
    m_showKeyboardAction->setProperty("componentDisplayName", i18n("Plasma Keyboard (custom)"));

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

    if (m_defaultLocale == locale) {
        m_defaultLocale.clear();
        Q_EMIT defaultLocaleChanged();
    }

    setNeedsSave(true);
}

QString PlasmaKeyboardKcm::defaultLocale() const
{
    return m_defaultLocale;
}

void PlasmaKeyboardKcm::setDefaultLocale(const QString &locale)
{
    // Only an enabled locale can open by default; an empty value hands the
    // choice back to Qt (the system locale, then the first enabled locale).
    if (locale == m_defaultLocale || (!locale.isEmpty() && !m_enabledLocales.contains(locale))) {
        return;
    }

    m_defaultLocale = locale;
    Q_EMIT defaultLocaleChanged();

    setNeedsSave(true);
}

void PlasmaKeyboardKcm::moveLocale(const QString &locale, int newIndex)
{
    const int oldIndex = m_enabledLocales.indexOf(locale);
    if (oldIndex < 0) {
        return;
    }

    const int targetIndex = qBound(0, newIndex, m_enabledLocales.size() - 1);
    if (oldIndex == targetIndex) {
        return;
    }

    m_enabledLocales.move(oldIndex, targetIndex);
    Q_EMIT enabledLocalesChanged();

    setNeedsSave(true);
}

QKeySequence PlasmaKeyboardKcm::defaultShortcut()
{
    return QKeySequence(Qt::META | Qt::SHIFT | Qt::Key_K);
}

QKeySequence PlasmaKeyboardKcm::shortcut() const
{
    return m_shortcut;
}

void PlasmaKeyboardKcm::loadShortcut()
{
    const QList<QKeySequence> shortcuts =
        KGlobalAccel::self()->globalShortcut(QStringLiteral("org.kde.plasma.keyboard.custom"), QStringLiteral("show-virtual-keyboard"));

    m_shortcut = shortcuts.isEmpty() ? defaultShortcut() : shortcuts.constFirst();
    if (m_shortcut.isEmpty()) {
        m_shortcut = defaultShortcut();
    }

    Q_EMIT shortcutChanged();
}

void PlasmaKeyboardKcm::setShortcut(const QKeySequence &shortcut)
{
    if (shortcut == m_shortcut) {
        return;
    }

    KGlobalAccel::self()->setShortcut(m_showKeyboardAction, QList<QKeySequence>{shortcut});
    m_shortcut = shortcut;
    Q_EMIT shortcutChanged();
}

void PlasmaKeyboardKcm::resetShortcut()
{
    setShortcut(defaultShortcut());
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

int PlasmaKeyboardKcm::floatingKeyboardWidthPercent() const
{
    return m_floatingKeyboardWidthPercent;
}

void PlasmaKeyboardKcm::setFloatingKeyboardWidthPercent(int percent)
{
    if (percent == m_floatingKeyboardWidthPercent) {
        return;
    }

    m_floatingKeyboardWidthPercent = percent;
    Q_EMIT floatingKeyboardWidthPercentChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::floatingKeyboardOpacity() const
{
    return m_floatingKeyboardOpacity;
}

void PlasmaKeyboardKcm::setFloatingKeyboardOpacity(int percent)
{
    if (percent == m_floatingKeyboardOpacity) {
        return;
    }

    m_floatingKeyboardOpacity = percent;
    Q_EMIT floatingKeyboardOpacityChanged();

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

bool PlasmaKeyboardKcm::gamepadAlternatesEnabled() const
{
    return m_gamepadAlternatesEnabled;
}

void PlasmaKeyboardKcm::setGamepadAlternatesEnabled(bool enabled)
{
    if (enabled == m_gamepadAlternatesEnabled) {
        return;
    }

    m_gamepadAlternatesEnabled = enabled;
    Q_EMIT gamepadAlternatesEnabledChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::gamepadAlternatesThresholdMs() const
{
    return m_gamepadAlternatesThresholdMs;
}

void PlasmaKeyboardKcm::setGamepadAlternatesThresholdMs(int thresholdMs)
{
    if (thresholdMs == m_gamepadAlternatesThresholdMs) {
        return;
    }

    m_gamepadAlternatesThresholdMs = thresholdMs;
    Q_EMIT gamepadAlternatesThresholdMsChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::predictiveTextEnabled() const
{
    return m_predictiveTextEnabled;
}

void PlasmaKeyboardKcm::setPredictiveTextEnabled(bool enabled)
{
    if (enabled == m_predictiveTextEnabled) {
        return;
    }

    m_predictiveTextEnabled = enabled;
    Q_EMIT predictiveTextEnabledChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::predictiveSuggestionCount() const
{
    return m_predictiveSuggestionCount;
}

void PlasmaKeyboardKcm::setPredictiveSuggestionCount(int count)
{
    if (count == m_predictiveSuggestionCount) {
        return;
    }

    m_predictiveSuggestionCount = count;
    Q_EMIT predictiveSuggestionCountChanged();

    setNeedsSave(true);
}

int PlasmaKeyboardKcm::predictiveMinPrefixLength() const
{
    return m_predictiveMinPrefixLength;
}

void PlasmaKeyboardKcm::setPredictiveMinPrefixLength(int length)
{
    if (length == m_predictiveMinPrefixLength) {
        return;
    }

    m_predictiveMinPrefixLength = length;
    Q_EMIT predictiveMinPrefixLengthChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::predictiveNextWordEnabled() const
{
    return m_predictiveNextWordEnabled;
}

void PlasmaKeyboardKcm::setPredictiveNextWordEnabled(bool enabled)
{
    if (enabled == m_predictiveNextWordEnabled) {
        return;
    }

    m_predictiveNextWordEnabled = enabled;
    Q_EMIT predictiveNextWordEnabledChanged();

    setNeedsSave(true);
}

bool PlasmaKeyboardKcm::predictiveTypoCorrectionEnabled() const
{
    return m_predictiveTypoCorrectionEnabled;
}

void PlasmaKeyboardKcm::setPredictiveTypoCorrectionEnabled(bool enabled)
{
    if (enabled == m_predictiveTypoCorrectionEnabled) {
        return;
    }

    m_predictiveTypoCorrectionEnabled = enabled;
    Q_EMIT predictiveTypoCorrectionEnabledChanged();

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
    setDefaultLocale(PlasmaKeyboardSettings::self()->defaultLocale());
    loadShortcut();
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
    setFloatingKeyboardWidthPercent(PlasmaKeyboardSettings::self()->floatingKeyboardWidthPercent());
    setFloatingKeyboardOpacity(PlasmaKeyboardSettings::self()->floatingKeyboardOpacity());
    setDiacriticsPopupEnabled(PlasmaKeyboardSettings::self()->diacriticsPopupEnabled());
    setDiacriticsHoldThresholdMs(PlasmaKeyboardSettings::self()->diacriticsHoldThresholdMs());
    setGamepadAlternatesEnabled(PlasmaKeyboardSettings::self()->gamepadAlternatesEnabled());
    setGamepadAlternatesThresholdMs(PlasmaKeyboardSettings::self()->gamepadAlternatesThresholdMs());
    setPredictiveTextEnabled(PlasmaKeyboardSettings::self()->predictiveTextEnabled());
    setPredictiveSuggestionCount(PlasmaKeyboardSettings::self()->predictiveSuggestionCount());
    setPredictiveMinPrefixLength(PlasmaKeyboardSettings::self()->predictiveMinPrefixLength());
    setPredictiveNextWordEnabled(PlasmaKeyboardSettings::self()->predictiveNextWordEnabled());
    setPredictiveTypoCorrectionEnabled(PlasmaKeyboardSettings::self()->predictiveTypoCorrectionEnabled());

    setNeedsSave(false);
}

void PlasmaKeyboardKcm::save()
{
    PlasmaKeyboardSettings::self()->setSoundEnabled(m_soundEnabled);
    PlasmaKeyboardSettings::self()->setVibrationEnabled(m_vibrationEnabled);
    PlasmaKeyboardSettings::self()->setVibrationStrength(m_vibrationStrength);
    PlasmaKeyboardSettings::self()->setEnabledLocales(m_enabledLocales);
    PlasmaKeyboardSettings::self()->setDefaultLocale(m_defaultLocale);
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
    PlasmaKeyboardSettings::self()->setFloatingKeyboardWidthPercent(m_floatingKeyboardWidthPercent);
    PlasmaKeyboardSettings::self()->setFloatingKeyboardOpacity(m_floatingKeyboardOpacity);
    PlasmaKeyboardSettings::self()->setDiacriticsPopupEnabled(m_diacriticsPopupEnabled);
    PlasmaKeyboardSettings::self()->setDiacriticsHoldThresholdMs(m_diacriticsHoldThresholdMs);
    PlasmaKeyboardSettings::self()->setGamepadAlternatesEnabled(m_gamepadAlternatesEnabled);
    PlasmaKeyboardSettings::self()->setGamepadAlternatesThresholdMs(m_gamepadAlternatesThresholdMs);
    PlasmaKeyboardSettings::self()->setPredictiveTextEnabled(m_predictiveTextEnabled);
    PlasmaKeyboardSettings::self()->setPredictiveSuggestionCount(m_predictiveSuggestionCount);
    PlasmaKeyboardSettings::self()->setPredictiveMinPrefixLength(m_predictiveMinPrefixLength);
    PlasmaKeyboardSettings::self()->setPredictiveNextWordEnabled(m_predictiveNextWordEnabled);
    PlasmaKeyboardSettings::self()->setPredictiveTypoCorrectionEnabled(m_predictiveTypoCorrectionEnabled);
    PlasmaKeyboardSettings::self()->save();

    setNeedsSave(false);
}

#include "plasmakeyboardkcm.moc"

#include "moc_plasmakeyboardkcm.cpp"
