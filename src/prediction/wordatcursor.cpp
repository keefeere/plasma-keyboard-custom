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
