/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "wordatcursor.h"

#include <QChar>

QString wordBeforeCursor(const QString &textBeforeCursor)
{
    int start = textBeforeCursor.size();
    while (start > 0) {
        const QChar character = textBeforeCursor.at(start - 1);
        if (character.isLetter()) {
            --start;
            continue;
        }
        // A hyphen or an apostrophe belongs to the word when it stands between
        // letters («что-то», «don't»), never at its beginning.
        if ((character == QChar(u'-') || character == QChar(u'\'')) && start >= 2 && textBeforeCursor.at(start - 2).isLetter()) {
            --start;
            continue;
        }
        break;
    }

    return textBeforeCursor.mid(start);
}

QString previousWordBeforeCursor(const QString &textBeforeCursor)
{
    // The word being typed is what stands right before the cursor; what is
    // looked for is the one before it.
    const QString current = wordBeforeCursor(textBeforeCursor);
    int end = textBeforeCursor.size() - current.size();

    // The separators between the two words (spaces, punctuation) are skipped,
    // the way wordBeforeCursor() skips them at the end of a word.
    while (end > 0) {
        const QChar character = textBeforeCursor.at(end - 1);
        if (character.isLetter() || character == QChar(u'-') || character == QChar(u'\'')) {
            break;
        }
        --end;
    }

    return wordBeforeCursor(textBeforeCursor.left(end));
}
