#include "database.h"
#include "macros.h"

#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#define ARENA_META_DB_URL "https://firestore.googleapis.com/v1beta1/projects/arenameta-3b1a7/databases/(default)/documents"

FirebaseDatabase::FirebaseDatabase(QObject *parent)
{

}

FirebaseDatabase::~FirebaseDatabase()
{

}

void FirebaseDatabase::updatePlayerCollection(QMap<int, int> ownedCards)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    QJsonObject jsonCollection;
    for (int key : ownedCards.keys()) {
        jsonCollection.insert(QString::number(key),
                              QJsonObject{{ "integerValue", QString::number(ownedCards[key]) }});
    }
    QJsonObject jsonObj{
        {"fields", QJsonObject{
                {"collection", QJsonObject{
                        {"mapValue", QJsonObject{
                                {"fields", jsonCollection}
                            }}
                    }}
            }}
    };
    QUrl url(QString("%1/users/%2?updateMask.fieldPaths=collection")
             .arg(ARENA_META_DB_URL).arg(userSettings.userId));
    createPatchRequest(url, QJsonDocument(jsonObj), userSettings.userToken);
}

void FirebaseDatabase::updateUserInventory(PlayerInventory playerInventory)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    QJsonObject jsonObj{
        {"fields", QJsonObject{
                {"inventory", QJsonObject{
                        {"mapValue", QJsonObject{
                                {"fields", QJsonObject{
                                        { "wcCommon", QJsonObject{
                                                { "integerValue", QString::number(playerInventory.wcCommon) }}},
                                        { "wcUncommon", QJsonObject{
                                                { "integerValue", QString::number(playerInventory.wcUncommon) }}},
                                        { "wcRare", QJsonObject{
                                                { "integerValue", QString::number(playerInventory.wcRare) }}},
                                        { "wcMythic", QJsonObject{
                                                { "integerValue", QString::number(playerInventory.wcMythic) }}},
                                        { "vaultProgress", QJsonObject{
                                                { "doubleValue", QString::number(playerInventory.vaultProgress) }}}
                                    }}
                            }}
                    }}
            }}
    };
    QUrl url(QString("%1/users/%2?updateMask.fieldPaths=inventory")
             .arg(ARENA_META_DB_URL).arg(userSettings.userId));
    createPatchRequest(url, QJsonDocument(jsonObj), userSettings.userToken);
}

void FirebaseDatabase::createPatchRequest(QUrl url, QJsonDocument body, QString userToken)
{
    LOGD(QString("Request: %1").arg(url.toString()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString("Authorization").toUtf8(),
                         QString("Bearer %1").arg(userToken).toUtf8());
    QBuffer *buffer = new QBuffer();
    buffer->open((QBuffer::ReadWrite));
    buffer->write(body.toJson());
    buffer->seek(0);
    QNetworkReply *reply = networkManager.sendCustomRequest(request, "PATCH", buffer);
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

    LOGD(QString("Database updated: &1").arg(reply->request().url().url()));
}
