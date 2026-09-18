/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "predictivedictionary.h"

#include <QFile>
#include <QLoggingCategory>
#include <QtEndian>

#include <algorithm>

Q_LOGGING_CATEGORY(lcPrediction, "org.kde.plasma.keyboard.custom.prediction")

namespace
{

//! The UTF-8 of «ё» and of «е»: the keyboards type «е» and the word lists keep
//! the «ё», so the two have to be compared as the same letter.
const QByteArrayView yo("\xd1\x91", 2);
const QByteArrayView ye("\xd0\xb5", 2);

//! The form a word is looked up in: lower case, with «ё» as «е».
QByteArray lookupKey(QByteArrayView word)
{
    QByteArray key(word.data(), word.size());
    key.replace(yo, ye);
    return key;
}

//! The same form for what has been typed.
QByteArray lookupKey(const QString &word)
{
    QByteArray key = word.toLower().toUtf8();
    key.replace(yo, ye);
    return key;
}

//! The language part of a locale ("ru_RU", "ru-RU" and "ru" all give "ru").
QString languageOf(const QString &locale)
{
    int separator = locale.indexOf(QChar(u'_'));
    if (separator < 0) {
        separator = locale.indexOf(QChar(u'-'));
    }
    const QString language = separator > 0 ? locale.left(separator) : locale;
    return language.toLower();
}

//! @p word with the case of @p prefix: what was typed in upper case stays in
//! upper case, a capitalised word keeps its capital letter.
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

} // namespace

quint32 PredictiveDictionary::WordList::offset(quint32 index) const
{
    return qFromLittleEndian<quint32>(data.constData() + 8 + qsizetype(index) * 4);
}

quint32 PredictiveDictionary::WordList::frequency(quint32 index) const
{
    const qsizetype frequencies = 8 + qsizetype(count + 1) * 4 + offset(count);
    return qFromLittleEndian<quint32>(data.constData() + frequencies + qsizetype(index) * 4);
}

QByteArrayView PredictiveDictionary::WordList::word(quint32 index) const
{
    const qsizetype pool = 8 + qsizetype(count + 1) * 4;
    const quint32 start = offset(index);
    return QByteArrayView(data.constData() + pool + start, offset(index + 1) - start);
}

PredictiveDictionary::PredictiveDictionary(QObject *parent)
    : QObject(parent)
{
}

PredictiveDictionary::~PredictiveDictionary() = default;

std::shared_ptr<const PredictiveDictionary::WordList> PredictiveDictionary::listFor(const QString &locale) const
{
    const QString language = languageOf(locale);
    if (language.isEmpty()) {
        return {};
    }

    const auto cached = m_lists.constFind(language);
    if (cached != m_lists.constEnd()) {
        return *cached;
    }

    auto list = std::make_shared<WordList>();

    QFile file(QStringLiteral(":/prediction/%1.bin").arg(language));
    if (file.open(QIODevice::ReadOnly)) {
        list->data = file.readAll();
        if (list->data.size() >= 8) {
            list->count = qFromLittleEndian<quint32>(list->data.constData() + 4);
        }
        if (!list->isValid()) {
            qCWarning(lcPrediction) << "unusable predictive word list" << language << list->data.size() << "bytes";
            list->data.clear();
            list->count = 0;
        }
    } else {
        qCWarning(lcPrediction) << "no predictive word list for" << language;
    }

    m_lists.insert(language, list);
    return list;
}

bool PredictiveDictionary::supports(const QString &locale) const
{
    const auto list = listFor(locale);
    return list && list->isValid();
}

QStringList PredictiveDictionary::complete(const QString &prefix, int limit, const QString &locale) const
{
    if (prefix.isEmpty() || limit <= 0) {
        return {};
    }

    const auto list = listFor(locale);
    if (!list || !list->isValid()) {
        return {};
    }

    const QByteArray needle = lookupKey(prefix);
    if (needle.isEmpty()) {
        return {};
    }

    // The words are ordered by the lookup key, so the words starting with the
    // prefix are one range of it: from the prefix itself up to the prefix with
    // a byte that cannot start a character after it.
    const auto lowerBound = [list](const QByteArray &value) {
        quint32 low = 0;
        quint32 high = list->count;
        while (low < high) {
            const quint32 middle = low + (high - low) / 2;
            if (lookupKey(list->word(middle)) < value) {
                low = middle + 1;
            } else {
                high = middle;
            }
        }
        return low;
    };

    QByteArray upper = needle;
    upper.append(char(0xff));

    const quint32 first = lowerBound(needle);
    const quint32 last = lowerBound(upper);

    QList<quint32> matches;
    matches.reserve(last - first);
    for (quint32 index = first; index < last; ++index) {
        matches.append(index);
    }

    const auto moreFrequent = [list](quint32 left, quint32 right) {
        return list->frequency(left) > list->frequency(right);
    };
    if (matches.size() > limit) {
        std::partial_sort(matches.begin(), matches.begin() + limit, matches.end(), moreFrequent);
        matches.resize(limit);
    } else {
        std::sort(matches.begin(), matches.end(), moreFrequent);
    }

    QStringList candidates;
    candidates.reserve(matches.size());
    for (const quint32 index : std::as_const(matches)) {
        candidates.append(matchCase(QString::fromUtf8(list->word(index)), prefix));
    }
    return candidates;
}
