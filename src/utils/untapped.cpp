#include "untapped.h"
#include "../macros.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

Untapped::Untapped(QObject *parent) : QObject(parent)
{
    dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
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

void Untapped::uploadLogFile(MatchInfo matchInfo, QStack<QString> matchLogMsgs)
{
    this->matchInfo = matchInfo;
    preparedMatchLogFile(matchLogMsgs);
    untappedAPI->requestS3PutUrl();
}

QString Untapped::preparedMatchLogFile(QStack<QString> matchLogMsgs)
{
    QFile logFile(dataDir + QDir::separator() + "log.txt");
    if (logFile.exists()) {
      logFile.remove();
    }
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    while(!matchLogMsgs.isEmpty()) {
        logFile.write(matchLogMsgs.pop().toUtf8());
        logFile.seek(logFile.size());
    }
    logFile.flush();
    logFile.close();
    return logFile.fileName();
}

QJsonDocument Untapped::preparedMatchDescriptor(QString timestamp)
{
    return QJsonDocument(
        QJsonObject({
            { "timestamp", timestamp },
            { "summarizedMessageCount", matchInfo.summarizedMessage },
            { "client", QString("lt-%1").arg(qApp->applicationVersion()) },
            { "mtgaVersion", LOTUS_TRACKER->mtgArena->getClientVersion() },
            { "upload_token", APP_SETTINGS->getUntappedAnonymousUploadToken() },
            { "event", QJsonObject({
              { "name", matchInfo.eventId },
              { "maxWins", eventPlayerCourse.eventId == matchInfo.eventId ? eventPlayerCourse.maxWins : NULL },
              { "maxLosses", eventPlayerCourse.eventId == matchInfo.eventId ? eventPlayerCourse.maxLosses : NULL },
              { "currentWins", eventPlayerCourse.eventId == matchInfo.eventId ? eventPlayerCourse.currentWins : NULL },
              { "currentLosses", eventPlayerCourse.eventId == matchInfo.eventId ? eventPlayerCourse.currentLosses : NULL },
              { "processedMatchIds", eventPlayerCourse.eventId == matchInfo.eventId ? eventPlayerCourse.processedMatchIds : QJsonArray() },
            })}
        })
    );
}

void Untapped::onS3PutInfo(QString putUrl, QString timestamp)
{
    QJsonDocument descriptor = preparedMatchDescriptor(timestamp);
    LOGI(QString(descriptor.toJson()));
    LOGI("Upload");
}
