#include "untapped.h"
#include "../macros.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

Untapped::Untapped(QObject *parent) : QObject(parent),
    dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
{
    untappedAPI = new UntappedAPI(this);
    setupUntappedAPIConnections();

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
