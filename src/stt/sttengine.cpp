/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "sttengine.h"

namespace PlasmaKeyboardStt
{

SttEngine::SttEngine(QObject *parent)
    : QObject(parent)
{
}

SttEngine::~SttEngine() = default;

} // namespace PlasmaKeyboardStt
