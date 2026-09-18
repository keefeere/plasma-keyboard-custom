/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QString>

/**
 * The word the cursor is at the end of, in @p textBeforeCursor.
 *
 * The word is what the predictive text input looks up: the letters before the
 * cursor, together with the hyphens and apostrophes that stand between them
 * («что-то», «don't»). A separator at the beginning of the word is not part of
 * it, and so is everything after the last letter.
 *
 * @param textBeforeCursor The text of the field from its beginning up to the
 * cursor.
 * @return The word, empty when the cursor is not right after one.
 */
QString wordBeforeCursor(const QString &textBeforeCursor);
