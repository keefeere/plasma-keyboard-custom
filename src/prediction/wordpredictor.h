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
 * The words that follow the word being typed, for the predictive text input.
 *
 * The lists are the bigrams of a language with their corpus frequencies,
 * compiled into the binary as Qt resources (src/prediction.qrc). They are
 * generated from the Tatoeba sentence corpus by
 * tools/generate-prediction-ngrams.sh.
 *
 * Like PredictiveDictionary, this class only answers questions about words:
 * when the predictions are shown and what is inserted into the field is decided
 * by the QML side. The words are looked up case-insensitively and «ё» is matched
 * as «е», the way the words are typed on the keyboard.
 */
class WordPredictor : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit WordPredictor(QObject *parent = nullptr);
    ~WordPredictor() override;

    /**
     * The words that follow @p previousWord, most frequent first.
     *
     * @param previousWord The last word of the text before the cursor. An empty
     * word has no predictions, and so has a locale without a bigram list.
     * @param limit The maximum number of words to return.
     * @param locale The input locale ("ru_RU", "en_US", ...); only the
     * language part is used.
     * @return Up to @p limit words, or an empty list when there is nothing to
     * predict.
     */
    Q_INVOKABLE QStringList predict(const QString &previousWord, int limit, const QString &locale) const;

    /**
     * Whether a bigram list for @p locale is compiled into the binary.
     */
    Q_INVOKABLE bool supports(const QString &locale) const;

    /**
     * One bigram list, loaded from the PKD2 resource.
     *
     * The resource is kept as it was read: the keys and the words are found in
     * their pools through the offsets, without copying them into any other
     * structure, and a word becomes a string only when it is offered as a
     * prediction.
     *
     * Internal, but public because the lookups are implemented outside of the
     * class.
     */
    struct NgramList {
        //! The whole resource: header, offsets, key pool, entries and word pool.
        QByteArray data;

        //! Number of keys (the words a bigram can start with).
        quint32 keyCount = 0;

        //! Number of continuation entries over all keys.
        quint32 entryCount = 0;

        //! Number of distinct continuation words.
        quint32 wordCount = 0;

        //! Whether the resource was understood at all.
        bool isValid() const;

        quint32 keyOffset(quint32 index) const;
        QByteArrayView key(quint32 index) const;
        quint32 entryOffset(quint32 index) const;
        quint32 entryWord(quint32 index) const;
        quint32 wordOffset(quint32 index) const;
        QByteArrayView word(quint32 index) const;

        /**
         * The index of the key @p needle, or keyCount when there is none.
         */
        quint32 findKey(const QByteArray &needle) const;

        qsizetype keyPoolOffset() const;
        qsizetype entryOffsetsOffset() const;
        qsizetype entryWordsOffset() const;
        qsizetype wordOffsetsOffset() const;
        qsizetype wordPoolOffset() const;
        qsizetype totalSize() const;
    };

private:
    /**
     * The bigram list of the language of @p locale, loaded from the resources
     * on first use. A null pointer when the language has none.
     */
    std::shared_ptr<const NgramList> listFor(const QString &locale) const;

    mutable QHash<QString, std::shared_ptr<const NgramList>> m_lists;
};
