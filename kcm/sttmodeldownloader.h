/*
    SPDX-FileCopyrightText: 2026 Aleksandr Kvintilyanov <bednyj.mops@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include <QFile>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

#include "sttmodelcatalog.h"

class QNetworkReply;

namespace PlasmaKeyboardStt
{

/**
 * Downloads one model from the catalog into the model directory.
 *
 * The download is written next to its final place, checked against the size and
 * the hash the catalog publishes and only then moved (or unpacked, for the
 * archives) into place, so a broken download never looks installed.
 */
class SttModelDownloader : public QObject
{
    Q_OBJECT

public:
    explicit SttModelDownloader(QObject *parent = nullptr);

    bool isBusy() const;
    QString modelId() const;
    qreal progress() const;
    QString error() const;

    void start(const SttModelEntry &entry);
    void cancel();

Q_SIGNALS:
    void stateChanged();
    void progressChanged();
    void errorChanged();
    void finished(const QString &modelId, bool ok, const QString &error);

private:
    void handleReadyRead();
    void handleFinished();
    void handleProgress(qint64 received, qint64 total);
    void fail(const QString &message);

    static bool verifyFile(const QString &path, const SttModelEntry &entry, QString *error);
    static bool unpackArchive(const QString &archive, const QString &destination, QString *error);

    QNetworkAccessManager m_manager;
    QNetworkReply *m_reply = nullptr;
    QFile m_file;
    SttModelEntry m_entry;
    QString m_partPath;
    QString m_error;
    qreal m_progress = 0;
    bool m_busy = false;
};

} // namespace PlasmaKeyboardStt
