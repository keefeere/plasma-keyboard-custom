/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "sttengine.h"

#include <QLibrary>

namespace PlasmaKeyboardStt
{

/**
 * Vosk through libvosk. The library is not linked into the keyboard: it is
 * loaded while the application runs, the same way the hunspell dictionary is,
 * so the package does not depend on vosk-api being installed. Without it the
 * engine is simply not available and the other engines keep working.
 *
 * A Vosk model is a directory, not a file, and the model itself decides the
 * language, so the language hint is not used.
 */
class VoskEngine : public SttEngine
{
    Q_OBJECT

public:
    explicit VoskEngine(QObject *parent = nullptr);
    ~VoskEngine() override;

    QString id() const override;
    QString displayName() const override;
    bool isAvailable(QString *reason = nullptr) const override;
    bool loadModel(const QString &modelPath, QString *error) override;
    void unloadModel() override;
    bool hasModel() const override;
    QString transcribe(const QVector<float> &samples, const QString &language, QString *error) override;

private:
    //! The library and the functions looked up in it. Loaded once per process.
    struct Api;
    static const Api *api();

    //! libvosk works with opaque handles; the header is not needed to build.
    void *m_model = nullptr;
    void *m_recognizer = nullptr;
    QString m_modelPath;
};

} // namespace PlasmaKeyboardStt
