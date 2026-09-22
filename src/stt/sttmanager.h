/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QFutureWatcher>
#include <QObject>
#include <QString>
#include <QThreadPool>
#include <QVector>

namespace PlasmaKeyboardStt
{

class SttAudioRecorder;
class SttEngine;

/**
 * The speech recognition the keyboard offers.
 *
 * It owns the microphone recorder and the selected engine, keeps them in sync
 * with the settings and runs the recognition on a worker thread, so the
 * keyboard keeps responding while a phrase is recognised. The recognised text
 * is handed over with textRecognized(); the keyboard inserts it into the field
 * it types into.
 *
 * The whole feature is optional: with `sttEnabled` off nothing is recorded and
 * no model is loaded.
 */
class SttManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool available READ isAvailable NOTIFY stateChanged)
    Q_PROPERTY(bool recording READ isRecording NOTIFY recordingChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(qreal level READ level NOTIFY levelChanged)
    Q_PROPERTY(QString engineId READ engineId WRITE setEngineId NOTIFY engineIdChanged)
    Q_PROPERTY(QString engineName READ engineName NOTIFY stateChanged)
    Q_PROPERTY(QString modelPath READ modelPath WRITE setModelPath NOTIFY modelPathChanged)
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit SttManager(QObject *parent = nullptr);
    ~SttManager() override;

    static SttManager *instance();

    bool isEnabled() const;
    void setEnabled(bool enabled);

    /*! Whether the selected engine can be used right now (library present, model set). */
    bool isAvailable() const;

    bool isRecording() const;
    bool isBusy() const;
    qreal level() const;

    QString engineId() const;
    void setEngineId(const QString &engineId);

    /*! Human readable name of the selected engine. */
    QString engineName() const;

    QString modelPath() const;
    void setModelPath(const QString &modelPath);

    /*! Language for the next recognition: the keyboard layout or the fixed one. */
    QString language() const;

    QString lastError() const;

    /*! Identifier of the engine that is used when nothing is configured. */
    static QString defaultEngineId();

    /*! Identifiers of all engines this build knows about. */
    static QStringList engineIds();

    /**
     * Starts recording. \a keyboardLanguage is the language of the active
     * keyboard layout; it is used when the language setting says the recognition
     * follows the keyboard.
     */
    Q_INVOKABLE void startRecording(const QString &keyboardLanguage = QString());

    /*! Stops recording and recognises what was recorded. */
    Q_INVOKABLE void stopRecording();

    /*! Stops recording and throws the audio away. */
    Q_INVOKABLE void cancel();

    /*! Drops the loaded model, so the next recognition loads it again. */
    Q_INVOKABLE void reloadModel();

Q_SIGNALS:
    void enabledChanged();
    void stateChanged();
    void recordingChanged();
    void busyChanged();
    void levelChanged();
    void engineIdChanged();
    void modelPathChanged();
    void languageChanged();
    void lastErrorChanged();
    void textRecognized(const QString &text);

private:
    void createEngine();
    void setLastError(const QString &message);
    QString recognize(const QVector<float> &samples);
    void handleRecorderError(const QString &message);

    static SttManager *s_instance;

    SttAudioRecorder *m_recorder = nullptr;
    SttEngine *m_engine = nullptr;
    QThreadPool m_pool;
    QString m_keyboardLanguage;
    QString m_lastError;
    bool m_busy = false;
    bool m_modelDirty = true;
};

} // namespace PlasmaKeyboardStt

//! Convenience name for the one place that creates the manager (main.cpp).
using SttManager = PlasmaKeyboardStt::SttManager;
