// SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include <QTest>

#include "hunspelldictionary.h"

/**
 * Tests the hunspell dictionaries of the system.
 *
 * Whether a dictionary is installed is up to the machine the test runs on, so
 * the checks that need one skip themselves when there is none; the checks that
 * need none always run.
 */
class HunspellDictionaryTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void knowsWhetherHunspellWasCompiledIn();
    void knowsNothingWithoutADictionary();
    void spellsAndSuggestsWithAnInstalledDictionary();
    void completesFromAnInstalledDictionary();

private:
    HunspellDictionary dictionary;
};

void HunspellDictionaryTest::knowsWhetherHunspellWasCompiledIn()
{
#ifdef HAVE_HUNSPELL
    QVERIFY(HunspellDictionary::isCompiledIn());
#else
    QVERIFY(!HunspellDictionary::isCompiledIn());
#endif
}

void HunspellDictionaryTest::knowsNothingWithoutADictionary()
{
    // A language no dictionary is installed for: every question is answered
    // with «no», without crashing.
    const QString locale = QStringLiteral("xx_XX");
    QVERIFY(!dictionary.isAvailable(locale));
    QVERIFY(!dictionary.spell(QStringLiteral("hello"), locale));
    QVERIFY(dictionary.suggest(QStringLiteral("helo"), 5, locale).isEmpty());
    QVERIFY(dictionary.complete(QStringLiteral("hel"), 5, locale).isEmpty());
}

void HunspellDictionaryTest::spellsAndSuggestsWithAnInstalledDictionary()
{
    QString locale;
    QString correct;
    QString typo;
    if (dictionary.isAvailable(QStringLiteral("ru_RU"))) {
        locale = QStringLiteral("ru_RU");
        correct = QStringLiteral("привет");
        typo = QStringLiteral("привт");
    } else if (dictionary.isAvailable(QStringLiteral("en_US"))) {
        locale = QStringLiteral("en_US");
        correct = QStringLiteral("hello");
        typo = QStringLiteral("helo");
    } else {
        QSKIP("no hunspell dictionary is installed");
    }

    QVERIFY(dictionary.spell(correct, locale));
    QVERIFY(!dictionary.spell(typo, locale));
    QVERIFY(dictionary.suggest(typo, 5, locale).contains(correct));
}

void HunspellDictionaryTest::completesFromAnInstalledDictionary()
{
    QString locale;
    QString prefix;
    if (dictionary.isAvailable(QStringLiteral("ru_RU"))) {
        locale = QStringLiteral("ru_RU");
        prefix = QStringLiteral("при");
    } else if (dictionary.isAvailable(QStringLiteral("en_US"))) {
        locale = QStringLiteral("en_US");
        prefix = QStringLiteral("hel");
    } else {
        QSKIP("no hunspell dictionary is installed");
    }

    const QStringList candidates = dictionary.complete(prefix, 5, locale);
    QVERIFY(!candidates.isEmpty());
    QVERIFY(candidates.size() <= 5);
    for (const QString &candidate : candidates) {
        QVERIFY(candidate.startsWith(prefix, Qt::CaseInsensitive));
    }
}

QTEST_GUILESS_MAIN(HunspellDictionaryTest)

#include "hunspelldictionarytest.moc"
