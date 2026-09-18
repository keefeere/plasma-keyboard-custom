// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QTest>

#include "predictivedictionary.h"

/**
 * Tests the word completion of the predictive text input.
 *
 * The expectations are the actual most frequent words of the compiled-in word
 * lists, so they fail when the lists are regenerated with other data.
 */
class PredictiveDictionaryTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void completesRussianWords();
    void completesEnglishWords();
    void completesHyphenatedRussianWords();
    void appliesTheCaseOfThePrefix();
    void neverReturnsMoreThanTheLimit();
    void looksUpYoAsYe();
    void suggestsNothingWithoutAPrefix();
    void suggestsNothingForAnUnknownLanguage();
    void knowsWhichLanguagesItHas();

private:
    PredictiveDictionary dictionary;
};

void PredictiveDictionaryTest::completesRussianWords()
{
    const QStringList candidates = dictionary.complete(QStringLiteral("прив"), 3, QStringLiteral("ru_RU"));
    QCOMPARE(candidates, QStringList({QStringLiteral("привет"), QStringLiteral("привести"), QStringLiteral("привело")}));
}

void PredictiveDictionaryTest::completesEnglishWords()
{
    const QStringList candidates = dictionary.complete(QStringLiteral("hel"), 3, QStringLiteral("en_US"));
    QCOMPARE(candidates, QStringList({QStringLiteral("help"), QStringLiteral("hello"), QStringLiteral("hell")}));
}

void PredictiveDictionaryTest::completesHyphenatedRussianWords()
{
    // Russian words are written with a hyphen and are kept in the word list.
    QCOMPARE(dictionary.complete(QStringLiteral("что-т"), 1, QStringLiteral("ru_RU")), QStringList({QStringLiteral("что-то")}));
    QCOMPARE(dictionary.complete(QStringLiteral("Что-т"), 1, QStringLiteral("ru_RU")), QStringList({QStringLiteral("Что-то")}));
}

void PredictiveDictionaryTest::appliesTheCaseOfThePrefix()
{
    const QStringList capitalized = dictionary.complete(QStringLiteral("Прив"), 2, QStringLiteral("ru_RU"));
    QCOMPARE(capitalized, QStringList({QStringLiteral("Привет"), QStringLiteral("Привести")}));

    const QStringList upperCase = dictionary.complete(QStringLiteral("HEL"), 1, QStringLiteral("en_US"));
    QCOMPARE(upperCase, QStringList({QStringLiteral("HELP")}));
}

void PredictiveDictionaryTest::neverReturnsMoreThanTheLimit()
{
    QCOMPARE(dictionary.complete(QStringLiteral("прив"), 2, QStringLiteral("ru_RU")).size(), 2);
    QCOMPARE(dictionary.complete(QStringLiteral("прив"), 1, QStringLiteral("ru_RU")).size(), 1);
    QVERIFY(dictionary.complete(QStringLiteral("прив"), 0, QStringLiteral("ru_RU")).isEmpty());
    QVERIFY(dictionary.complete(QStringLiteral("прив"), -1, QStringLiteral("ru_RU")).isEmpty());
}

void PredictiveDictionaryTest::looksUpYoAsYe()
{
    // Typed with an «е»: the words with an «ё» are offered all the same (the
    // word itself is the 31st most frequent one of the «еж» words).
    const QStringList candidates = dictionary.complete(QStringLiteral("еж"), 100, QStringLiteral("ru_RU"));
    QVERIFY(candidates.contains(QStringLiteral("ёж")));

    // The other way around: a typed «ё» is looked up as an «е».
    QCOMPARE(dictionary.complete(QStringLiteral("ёж"), 1, QStringLiteral("ru_RU")), QStringList({QStringLiteral("ежедневно")}));
}

void PredictiveDictionaryTest::suggestsNothingWithoutAPrefix()
{
    QVERIFY(dictionary.complete(QString(), 3, QStringLiteral("ru_RU")).isEmpty());
}

void PredictiveDictionaryTest::suggestsNothingForAnUnknownLanguage()
{
    QVERIFY(dictionary.complete(QStringLiteral("str"), 3, QStringLiteral("de_DE")).isEmpty());
    QVERIFY(dictionary.complete(QStringLiteral("str"), 3, QString()).isEmpty());
}

void PredictiveDictionaryTest::knowsWhichLanguagesItHas()
{
    QVERIFY(dictionary.supports(QStringLiteral("ru_RU")));
    QVERIFY(dictionary.supports(QStringLiteral("en")));
    QVERIFY(dictionary.supports(QStringLiteral("en-US")));
    QVERIFY(!dictionary.supports(QStringLiteral("de_DE")));
}

QTEST_GUILESS_MAIN(PredictiveDictionaryTest)

#include "predictivedictionarytest.moc"
