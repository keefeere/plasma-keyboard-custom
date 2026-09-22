/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QObject>
#include <QString>
#include <QVector>

namespace PlasmaKeyboardStt
{

/**
 * One speech recognition backend.
 *
 * Every engine takes 16 kHz mono floating point samples and returns the text it
 * recognised. The model is loaded separately, because loading it is expensive
 * and the engines keep the loaded model between the recognitions.
 */
class SttEngine : public QObject
{
    Q_OBJECT

public:
    explicit SttEngine(QObject *parent = nullptr);
    ~SttEngine() override;

    /*! Identifier used in the settings, for example "parakeet". */
    virtual QString id() const = 0;

    /*! Human readable name, shown in the settings. */
    virtual QString displayName() const = 0;

    /**
     * Whether the engine can be used at all: the library it needs is present.
     * An engine that is compiled into the keyboard is always available; an
     * engine loaded while the application runs depends on the library being
     * installed. \a reason receives a short explanation when it is not.
     */
    virtual bool isAvailable(QString *reason = nullptr) const = 0;

    /*! Loads the model from \a modelPath. Returns false and sets \a error on failure. */
    virtual bool loadModel(const QString &modelPath, QString *error) = 0;

    /*! Drops the loaded model and frees its memory. */
    virtual void unloadModel() = 0;

    /*! Whether a model is loaded right now. */
    virtual bool hasModel() const = 0;

    /**
     * Recognises \a samples (16 kHz, mono, -1..1) and returns the text.
     * \a language is the language hint; an empty value lets the engine decide.
     * An engine that does not support the language hint ignores it.
     */
    virtual QString transcribe(const QVector<float> &samples, const QString &language, QString *error) = 0;
};

} // namespace PlasmaKeyboardStt
