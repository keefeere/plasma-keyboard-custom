// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QTest>

#include "wordatcursor.h"

/**
 * Tests the word the predictive text input is offered for, as it is cut out of
 * the text in front of the cursor.
 */
class WordAtCursorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void takesTheWordBeforeTheCursor();
    void stopsAtWhatIsNotPartOfAWord();
    void keepsSeparatorsInsideWords();
    void keepsSeparatorsAtTheEndOfWords();
    void doesNotStartAWordWithASeparator();
    void knowsLettersOutsideLatinAndCyrillic();

private:
    static QString word(const QString &text)
    {
        return wordBeforeCursor(text);
    }
};

void WordAtCursorTest::takesTheWordBeforeTheCursor()
{
    QCOMPARE(word(QStringLiteral("привет")), QStringLiteral("привет"));
    QCOMPARE(word(QStringLiteral("привет, ми")), QStringLiteral("ми"));
    QCOMPARE(word(QStringLiteral("hello wor")), QStringLiteral("wor"));
    QCOMPARE(word(QStringLiteral("Ёж")), QStringLiteral("Ёж"));
}

void WordAtCursorTest::stopsAtWhatIsNotPartOfAWord()
{
    // Nothing to continue: after a space, after punctuation, in numbers.
    QCOMPARE(word(QStringLiteral("привет, мир ")), QString());
    QCOMPARE(word(QStringLiteral("привет!")), QString());
    QCOMPARE(word(QStringLiteral("123 456")), QString());
    QCOMPARE(word(QString()), QString());
}

void WordAtCursorTest::keepsSeparatorsInsideWords()
{
    QCOMPARE(word(QStringLiteral("что-то")), QStringLiteral("что-то"));
    QCOMPARE(word(QStringLiteral("по-мое")), QStringLiteral("по-мое"));
    QCOMPARE(word(QStringLiteral("don't")), QStringLiteral("don't"));
    QCOMPARE(word(QStringLiteral("it's a don't")), QStringLiteral("don't"));
}

void WordAtCursorTest::keepsSeparatorsAtTheEndOfWords()
{
    // A separator that has just been typed belongs to the word: the words that
    // continue it start with it too («что-то»).
    QCOMPARE(word(QStringLiteral("что-")), QStringLiteral("что-"));
    QCOMPARE(word(QStringLiteral("don'")), QStringLiteral("don'"));
}

void WordAtCursorTest::doesNotStartAWordWithASeparator()
{
    QCOMPARE(word(QStringLiteral("-что")), QStringLiteral("что"));
    QCOMPARE(word(QStringLiteral("'s")), QStringLiteral("s"));
    QCOMPARE(word(QStringLiteral("-- ")), QString());
}

void WordAtCursorTest::knowsLettersOutsideLatinAndCyrillic()
{
    QCOMPARE(word(QStringLiteral("café")), QStringLiteral("café"));
    QCOMPARE(word(QStringLiteral("日本語")), QStringLiteral("日本語"));
}

QTEST_GUILESS_MAIN(WordAtCursorTest)

#include "wordatcursortest.moc"
