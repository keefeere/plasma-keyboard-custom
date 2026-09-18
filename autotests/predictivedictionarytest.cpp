// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QElapsedTimer>
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
    void correctsAMistypedRussianWord();
    void correctsAMistypedEnglishWord();
    void appliesTheCaseToACorrection();
    void correctsNothingForAnUnknownLanguage();
    void neverCorrectsMoreThanTheLimit();
    void correctsQuickly();

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

void PredictiveDictionaryTest::correctsAMistypedRussianWord()
{
    // A letter is missing, a letter is swapped with its neighbour and a letter
    // is typed instead of the right one.
    QCOMPARE(dictionary.correct(QStringLiteral("привт"), 1, QStringLiteral("ru_RU")), QStringList({QStringLiteral("привет")}));
    QVERIFY(dictionary.correct(QStringLiteral("првиет"), 3, QStringLiteral("ru_RU")).contains(QStringLiteral("привет")));
    QVERIFY(dictionary.correct(QStringLiteral("превет"), 3, QStringLiteral("ru_RU")).contains(QStringLiteral("привет")));
    // A letter is typed too many.
    QVERIFY(dictionary.correct(QStringLiteral("привтет"), 3, QStringLiteral("ru_RU")).contains(QStringLiteral("привет")));
}

void PredictiveDictionaryTest::correctsAMistypedEnglishWord()
{
    QVERIFY(dictionary.correct(QStringLiteral("helo"), 5, QStringLiteral("en_US")).contains(QStringLiteral("hello")));
    QVERIFY(dictionary.correct(QStringLiteral("thier"), 5, QStringLiteral("en_US")).contains(QStringLiteral("their")));
}

void PredictiveDictionaryTest::appliesTheCaseToACorrection()
{
    QCOMPARE(dictionary.correct(QStringLiteral("Привт"), 1, QStringLiteral("ru_RU")), QStringList({QStringLiteral("Привет")}));
}

void PredictiveDictionaryTest::correctsNothingForAnUnknownLanguage()
{
    QVERIFY(dictionary.correct(QStringLiteral("привт"), 3, QStringLiteral("de_DE")).isEmpty());
    QVERIFY(dictionary.correct(QStringLiteral("привт"), 3, QString()).isEmpty());
}

void PredictiveDictionaryTest::neverCorrectsMoreThanTheLimit()
{
    QVERIFY(dictionary.correct(QStringLiteral("превет"), 2, QStringLiteral("ru_RU")).size() <= 2);
    QVERIFY(dictionary.correct(QStringLiteral("превет"), 1, QStringLiteral("ru_RU")).size() <= 1);
    QVERIFY(dictionary.correct(QStringLiteral("превет"), 0, QStringLiteral("ru_RU")).isEmpty());
    QVERIFY(dictionary.correct(QStringLiteral("превет"), -1, QStringLiteral("ru_RU")).isEmpty());
}

void PredictiveDictionaryTest::correctsQuickly()
{
    // The corrections are looked for on every letter that is typed, so the
    // lookup has to stay far below the time a key press takes. A hundred
    // corrections of a seven letter word is a generous upper bound.
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; i < 100; ++i) {
        dictionary.correct(QStringLiteral("превет"), 3, QStringLiteral("ru_RU"));
    }
    const qint64 elapsed = timer.elapsed();
    qInfo() << "100 corrections of a seven letter word:" << elapsed << "ms";
    QVERIFY(elapsed < 2000);
}

QTEST_GUILESS_MAIN(PredictiveDictionaryTest)

#include "predictivedictionarytest.moc"
