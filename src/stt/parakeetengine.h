/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "sttengine.h"

struct parakeet_context;

namespace PlasmaKeyboardStt
{

/**
 * NVIDIA Parakeet TDT 0.6B v3 through libparakeet, the engine that ships with
 * whisper.cpp. The library is built together with the keyboard, so this engine
 * is always available.
 *
 * The model is a GGUF file. The v3 model is multilingual, so no language hint
 * is passed to it.
 */
class ParakeetEngine : public SttEngine
{
    Q_OBJECT

public:
    explicit ParakeetEngine(QObject *parent = nullptr);
    ~ParakeetEngine() override;

    QString id() const override;
    QString displayName() const override;
    bool isAvailable(QString *reason = nullptr) const override;
    bool loadModel(const QString &modelPath, QString *error) override;
    void unloadModel() override;
    bool hasModel() const override;
    QString transcribe(const QVector<float> &samples, const QString &language, QString *error) override;

private:
    struct parakeet_context *m_context = nullptr;
    QString m_modelPath;
};

} // namespace PlasmaKeyboardStt
