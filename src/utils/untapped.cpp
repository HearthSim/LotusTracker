#include "untapped.h"
#include "../macros.h"

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>

#define TAR_PATH "bin/tar.exe"
#define XZ_PATH "bin/xz.exe"

Untapped::Untapped(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    tempDir = QFile(dataDir + QDir::separator() + "temp").fileName();
    QDir dir(tempDir);
    if (!dir.exists() || dir.isEmpty()) {
        QDir dir;
        dir.mkpath(tempDir);
    }

    untappedAPI = new UntappedAPI(this);
    setupUntappedAPIConnections();
}

void Untapped::setupUntappedAPIConnections()
{
    connect(untappedAPI, &UntappedAPI::sgnS3PutInfo, this, &Untapped::onS3PutInfo);
    connect(untappedAPI, &UntappedAPI::sgnNewAnonymousUploadToken,
            this, [](QString uploadToken){
        APP_SETTINGS->setUntappedAnonymousUploadToken(uploadToken);
    });
}

void Untapped::checkForUntappedUploadToken()
{
    if (APP_SETTINGS->getUntappedAnonymousUploadToken().isEmpty()) {
        untappedAPI->fetchAnonymousUploadToken();
    }
}

void Untapped::setEventPlayerCourse(EventPlayerCourse eventPlayerCourse)
{
    this->eventPlayerCourse = eventPlayerCourse;
}

void Untapped::uploadMatchToUntapped(MatchDetails matchDetails, QStack<QString> matchLogMsgs)
{
    if (matchDetails.games.isEmpty() || matchDetails.games.first().duration == 0) {
        influx_metric(influxdb_cpp::builder()
            .meas("lt_match_without_games")
            .tag("event", matchDetails.eventId.toStdString())
            .field("matchId", matchDetails.matchId.toStdString())
            .field("count", 1)
        );
        return;
    }
    this->matchDetails = matchDetails;
    prepareMatchLogFile(matchLogMsgs);
    untappedAPI->requestS3PutUrl();
}

void Untapped::prepareMatchLogFile(QStack<QString> matchLogMsgs)
{
    QFile logFile(tempDir + QDir::separator() + "log.txt");
    if (logFile.exists()) {
      logFile.remove();
    }
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    logFile.seek(0);
    while(!matchLogMsgs.isEmpty()) {
        logFile.write(matchLogMsgs.pop().toUtf8());
    }
    logFile.flush();
    logFile.close();
    QProcess::execute(XZ_PATH, QStringList() << "-f" << logFile.fileName());
}

void Untapped::prepareMatchDescriptor(QString timestamp, QString uploadToken)
{
    QJsonDocument descriptor = untappedMatchDescriptor.prepareNewDescriptor(matchDetails, timestamp,
                                                                            uploadToken, eventPlayerCourse);
    QFile descriptorFile(tempDir + QDir::separator() + "descriptor.json");
    if (descriptorFile.exists()) {
      descriptorFile.remove();
    }
    descriptorFile.open(QIODevice::WriteOnly | QIODevice::Text);
    descriptorFile.write(descriptor.toJson());
    descriptorFile.flush();
    descriptorFile.close();
}

void Untapped::onS3PutInfo(QString putUrl, QString timestamp)
{
    QString uploadToken = APP_SETTINGS->getUntappedAnonymousUploadToken();
    if (uploadToken.size() != 36) {
        LOGI("Invalid upload token");
        influx_metric(influxdb_cpp::builder()
            .meas("lt_untapped_log_upload_failed")
            .tag("reason", "Invalid Upload Token")
            .tag("matchId", matchDetails.matchId.toStdString())
            .field("count", 1)
            .field("uploadToken", uploadToken.toStdString())
        );
        APP_SETTINGS->clearUntappedAnonymousUploadToken();
        return;
    }
    prepareMatchDescriptor(timestamp, uploadToken);
    QByteArray uploadData = getUploadData();
    if (uploadData.isEmpty()) {
        LOGI("Error while packing data for upload");
        influx_metric(influxdb_cpp::builder()
            .meas("lt_untapped_log_upload_failed")
            .tag("reason", "Packing")
            .tag("matchId", matchDetails.matchId.toStdString())
            .field("count", 1)
        );
        return;
    }
    UntappedUploadData untappedUploadData(matchDetails.matchId, putUrl, uploadData);
    untappedAPI->uploadMatch(untappedUploadData);
}

QByteArray Untapped::getUploadData()
{
    QFile packedFile(tempDir + QDir::separator() + "match.packed");
    if (packedFile.exists()) {
      packedFile.remove();
    }
    QStringList args;
    args << "-cf";
    args << packedFile.fileName();
    args << "-C";
    args << tempDir;
    args << "descriptor.json";
    args << "log.txt.xz";
    QProcess::execute(TAR_PATH, args);
    QFile(tempDir + QDir::separator() + "descriptor.json").remove();
    QFile(tempDir + QDir::separator() + "log.txt.xz").remove();

    if (packedFile.exists()) {
        packedFile.open(QIODevice::ReadOnly);
        packedFile.seek(0);
        return packedFile.readAll();
    } else {
        return QByteArray();
    }
}
