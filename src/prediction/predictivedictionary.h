/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QByteArray>
#include <QByteArrayView>
#include <QHash>
#include <QObject>
#include <QStringList>
#include <memory>

#include <qqmlintegration.h>

/**
 * Word completion for the predictive text input.
 *
 * The word lists are the words of a language with their corpus frequencies,
 * compiled into the binary as Qt resources (src/prediction.qrc). They are
 * generated from the FrequencyWords word lists by
 * tools/generate-prediction-dictionaries.sh.
 *
 * This class only answers questions about words: what the row above the
 * keyboard shows and what is inserted into the field is decided by the QML
 * side. The words are looked up case-insensitively and «ё» is matched as «е»,
 * the way the words are typed on the keyboard.
 */
class PredictiveDictionary : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PredictiveDictionary(QObject *parent = nullptr);
    ~PredictiveDictionary() override;

    /**
     * The words starting with @p prefix, most frequent first.
     *
     * @param prefix What has been typed so far. An empty prefix has no
     * candidates, and so has a locale without a word list.
     * @param limit The maximum number of words to return.
     * @param locale The input locale ("ru_RU", "en_US", ...); only the
     * language part is used.
     * @return Up to @p limit words, with the case of @p prefix applied to
     * them, or an empty list when there is nothing to suggest.
     */
    Q_INVOKABLE QStringList complete(const QString &prefix, int limit, const QString &locale) const;

    /**
     * Whether a word list for @p locale is compiled into the binary.
     */
    Q_INVOKABLE bool supports(const QString &locale) const;

    /**
     * One word list, loaded from the PKD1 resource.
     *
     * The resource is kept as it was read: the words are found in the pool
     * through the offsets, without copying them into any other structure, and
     * a word becomes a string only when it is offered as a candidate.
     *
     * Internal, but public because the lookups are implemented outside of the
     * class.
     */
    struct WordList {
        //! The whole resource: header, offsets, pool and frequencies.
        QByteArray data;

        //! Number of words in the list.
        quint32 count = 0;

        //! Whether the resource was understood at all.
        bool isValid() const
        {
            if (count == 0 || !data.startsWith("PKD1")) {
                return false;
            }
            const qsizetype offsetsEnd = 8 + qsizetype(count + 1) * 4;
            if (data.size() < offsetsEnd) {
                return false;
            }
            const quint32 poolSize = offset(count);
            return data.size() >= offsetsEnd + qsizetype(poolSize) + qsizetype(count) * 4;
        }

        quint32 offset(quint32 index) const;
        quint32 frequency(quint32 index) const;
        QByteArrayView word(quint32 index) const;
    };

private:
    /**
     * The word list of the language of @p locale, loaded from the resources on
     * first use. A null pointer when the language has none.
     */
    std::shared_ptr<const WordList> listFor(const QString &locale) const;

    mutable QHash<QString, std::shared_ptr<const WordList>> m_lists;
};
