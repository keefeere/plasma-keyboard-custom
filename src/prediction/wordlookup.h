/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QByteArray>
#include <QByteArrayView>
#include <QString>

/**
 * How a typed word is matched against the word lists of the predictive text
 * input.
 *
 * The word lists (the .bin dictionaries and the .ngrams bigram lists of
 * src/prediction/data) are ordered by the lookup key, and what is typed is
 * turned into the same key before it is compared with them. Both the word lists
 * and the engine then work with one form of a word.
 */

/**
 * The form @p word is looked up in: lower case, with «ё» as «е».
 */
QByteArray lookupKey(QByteArrayView word);
QByteArray lookupKey(const QString &word);

/**
 * The language part of a locale ("ru_RU", "ru-RU" and "ru" all give "ru").
 */
QString languageOf(const QString &locale);

/**
 * @p word with the case of @p prefix: what was typed in upper case stays in
 * upper case, a capitalised word keeps its capital letter.
 */
QString matchCase(const QString &word, const QString &prefix);
