// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

.pragma library

// Country flag emoji for a locale such as "en_US", the same visual the system
// keyboard KCM uses (KCountryFlagEmojiIconEngine). Locales without a country
// (for example "eo") have no flag, so an empty string is returned.
function flagForLocale(locale) {
    const parts = String(locale).split("_");
    if (parts.length < 2 || parts[1].length !== 2) {
        return "";
    }

    const country = parts[1].toUpperCase();
    const regionalIndicatorA = 0x1F1E6;
    return String.fromCodePoint(regionalIndicatorA + country.charCodeAt(0) - 65, regionalIndicatorA + country.charCodeAt(1) - 65);
}
