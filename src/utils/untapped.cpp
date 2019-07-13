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

void Untapped::preparedMatchDescriptor(QString timestamp)
{
    QJsonDocument descriptor(
        QJsonObject({
            { "timestamp", timestamp },
            { "summarizedMessageCount", matchInfo.summarizedMessage },
            { "client", QString("lt-%1").arg(qApp->applicationVersion()) },
            { "mtgaVersion", LOTUS_TRACKER->mtgArena->getClientVersion() },
            { "upload_token", APP_SETTINGS->getUntappedAnonymousUploadToken() },
            { "event", QJsonObject({
              { "name", matchInfo.eventId },
              { "maxWins", eventCourseIntToJsonValue(eventPlayerCourse.maxWins) },
              { "maxLosses", eventCourseIntToJsonValue(eventPlayerCourse.maxLosses) },
              { "currentWins", eventCourseIntToJsonValue(eventPlayerCourse.currentWins) },
              { "currentLosses", eventCourseIntToJsonValue(eventPlayerCourse.currentLosses) },
              { "processedMatchIds", eventPlayerCourse.eventId == matchInfo.eventId ? eventPlayerCourse.processedMatchIds : QJsonArray() },
            })}
        })
    );
    QFile descriptorFile(tempDir + QDir::separator() + "descriptor.json");
    if (descriptorFile.exists()) {
      descriptorFile.remove();
    }
    descriptorFile.open(QIODevice::WriteOnly | QIODevice::Text);
    descriptorFile.write(descriptor.toJson());
    descriptorFile.flush();
    descriptorFile.close();
}

QJsonValue Untapped::eventCourseIntToJsonValue(int value)
{
    if (eventPlayerCourse.eventId != matchInfo.eventId || value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}

void Untapped::onS3PutInfo(QString putUrl, QString timestamp)
{
    QJsonDocument descriptor = preparedMatchDescriptor(timestamp);
    LOGI(QString(descriptor.toJson()));
    LOGI("Upload");
}
