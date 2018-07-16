#include "database.h"
#include "macros.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#define ARENA_META_DB_URL "https://arenameta-3b1a7.firebaseio.com"

FirebaseDatabase::FirebaseDatabase(QObject *parent)
{

}

FirebaseDatabase::~FirebaseDatabase()
{

}

void FirebaseDatabase::updateUserInventory(PlayerInventory playerInventory)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }

    QJsonObject jsonObj;
    jsonObj.insert("wcCommon", playerInventory.wcCommon);
    jsonObj.insert("wcUncommon", playerInventory.wcUncommon);
    jsonObj.insert("wcRare", playerInventory.wcRare);
    jsonObj.insert("wcMythic", playerInventory.wcMythic);
    jsonObj.insert("vaultProgress", playerInventory.vaultProgress);
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(QString("%1/users/%2/inventory.json").arg(ARENA_META_DB_URL).arg(userSettings.userId));
    url.setQuery(QUrlQuery(QString("auth=%1").arg(userSettings.userToken)));
    LOGD(QString("Request: %1").arg(url.toString()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.put(request, body);
    connect(reply, &QNetworkReply::finished, this, &FirebaseDatabase::requestOnFinish);
}

void FirebaseDatabase::requestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    emit sgnRequestFinished();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1").arg(reason));
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
        ARENA_TRACKER->showMessage(message);
        return;
    }

    ARENA_TRACKER->showMessage("Inventory updated");
}
