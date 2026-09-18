// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QTest>

#include "wordpredictor.h"

/**
 * Tests the next-word prediction of the predictive text input.
 *
 * The expectations are the actual most frequent bigrams of the compiled-in
 * bigram lists, so they fail when the lists are regenerated with other data.
 */
class WordPredictorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void predictsRussianWords();
    void predictsEnglishWords();
    void looksUpYoAsYe();
    void neverReturnsMoreThanTheLimit();
    void predictsNothingWithoutAWord();
    void predictsNothingForAnUnknownLanguage();
    void knowsWhichLanguagesItHas();

private:
    WordPredictor predictor;
};

void WordPredictorTest::predictsRussianWords()
{
    QCOMPARE(predictor.predict(QStringLiteral("привет"), 3, QStringLiteral("ru_RU")),
             QStringList({QStringLiteral("я"), QStringLiteral("том"), QStringLiteral("меня")}));
    QCOMPARE(predictor.predict(QStringLiteral("спасибо"), 2, QStringLiteral("ru_RU")), QStringList({QStringLiteral("что"), QStringLiteral("за")}));
}

void WordPredictorTest::predictsEnglishWords()
{
    QCOMPARE(predictor.predict(QStringLiteral("thank"), 3, QStringLiteral("en_US")),
             QStringList({QStringLiteral("you"), QStringLiteral("god"), QStringLiteral("tom")}));
    QCOMPARE(predictor.predict(QStringLiteral("how"), 2, QStringLiteral("en_US")), QStringList({QStringLiteral("to"), QStringLiteral("many")}));
}

void WordPredictorTest::looksUpYoAsYe()
{
    // A key written with an «ё» is looked up as an «е», the way the words are
    // typed on the keyboard.
    const QStringList withYo = predictor.predict(QStringLiteral("всё"), 3, QStringLiteral("ru_RU"));
    QVERIFY(!withYo.isEmpty());
    QCOMPARE(withYo, predictor.predict(QStringLiteral("все"), 3, QStringLiteral("ru_RU")));
}

void WordPredictorTest::neverReturnsMoreThanTheLimit()
{
    QVERIFY(predictor.predict(QStringLiteral("привет"), 2, QStringLiteral("ru_RU")).size() <= 2);
    QVERIFY(predictor.predict(QStringLiteral("привет"), 1, QStringLiteral("ru_RU")).size() <= 1);
    QVERIFY(predictor.predict(QStringLiteral("привет"), 0, QStringLiteral("ru_RU")).isEmpty());
    QVERIFY(predictor.predict(QStringLiteral("привет"), -1, QStringLiteral("ru_RU")).isEmpty());
}

void WordPredictorTest::predictsNothingWithoutAWord()
{
    QVERIFY(predictor.predict(QString(), 3, QStringLiteral("ru_RU")).isEmpty());
}

void WordPredictorTest::predictsNothingForAnUnknownLanguage()
{
    QVERIFY(predictor.predict(QStringLiteral("hello"), 3, QStringLiteral("de_DE")).isEmpty());
    QVERIFY(predictor.predict(QStringLiteral("hello"), 3, QString()).isEmpty());
}

void WordPredictorTest::knowsWhichLanguagesItHas()
{
    QVERIFY(predictor.supports(QStringLiteral("ru_RU")));
    QVERIFY(predictor.supports(QStringLiteral("en")));
    QVERIFY(predictor.supports(QStringLiteral("en-US")));
    QVERIFY(!predictor.supports(QStringLiteral("de_DE")));
}

QTEST_GUILESS_MAIN(WordPredictorTest)

#include "wordpredictortest.moc"
