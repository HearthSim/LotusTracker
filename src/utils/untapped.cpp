#include "untapped.h"
#include "../macros.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

Untapped::Untapped(QObject *parent) : QObject(parent)
{
    dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    untappedAPI = new UntappedAPI(this);
    setupUntappedAPIConnections();
}

void Untapped::checkForUntappedUploadToken()
{
    if (APP_SETTINGS->getUntappedAnonymousUploadToken().isEmpty()) {
        untappedAPI->fetchAnonymousUploadToken();
    }
}

void Untapped::setupUntappedAPIConnections()
{
    connect(untappedAPI, &UntappedAPI::sgnNewAnonymousUploadToken,
            this, [](QString uploadToken){
        APP_SETTINGS->setUntappedAnonymousUploadToken(uploadToken);
    });
}

void Untapped::preparedMatchLogFile(QStack<QString> matchLogMsgs)
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
}
