/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "wordlookup.h"

#include <QChar>

//! The UTF-8 of «ё» and of «е»: the keyboards type «е» and the word lists keep
//! the «ё», so the two have to be compared as the same letter.
const QByteArrayView yo("\xd1\x91", 2);
const QByteArrayView ye("\xd0\xb5", 2);

QByteArray lookupKey(QByteArrayView word)
{
    QByteArray key(word.data(), word.size());
    key.replace(yo, ye);
    return key;
}

QByteArray lookupKey(const QString &word)
{
    QByteArray key = word.toLower().toUtf8();
    key.replace(yo, ye);
    return key;
}

QString languageOf(const QString &locale)
{
    int separator = locale.indexOf(QChar(u'_'));
    if (separator < 0) {
        separator = locale.indexOf(QChar(u'-'));
    }
    const QString language = separator > 0 ? locale.left(separator) : locale;
    return language.toLower();
}

QString matchCase(const QString &word, const QString &prefix)
{
    bool startsUpper = false;
    bool allUpper = true;
    for (const QChar character : prefix) {
        if (!character.isLetter()) {
            continue;
        }
        startsUpper = startsUpper || character.isUpper();
        allUpper = allUpper && character.isUpper();
    }

    if (allUpper && prefix.size() > 1) {
        return word.toUpper();
    }
    if (startsUpper && !word.isEmpty()) {
        QString capitalized = word;
        capitalized[0] = capitalized.at(0).toUpper();
        return capitalized;
    }
    return word;
}
