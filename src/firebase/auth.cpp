#include "auth.h"
#include "../apikeys.h"
#include "../macros.h"

#include <ctime>
#include <chrono>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#define REGISTER_URL "https://www.googleapis.com/identitytoolkit/v3/relyingparty/signupNewUser"

Auth::Auth(QObject *parent) : QObject(parent)
{

}

void Auth::registerUser(QString email, QString password)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("password", QJsonValue(password));
    jsonObj.insert("returnSecureToken", QJsonValue(true));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(REGISTER_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &Auth::registerOnFinish);
}

void Auth::registerOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    LOGD(QString(QJsonDocument(jsonRsp).toJson()));

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1").arg(reason));
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
        if (message == "EMAIL_EXISTS") {
            ARENA_TRACKER->showMessage(tr("Email already in use. Try do login."));
        }
        return;
    }

    QString userId = jsonRsp["localId"].toString();
    QString userToken = jsonRsp["idToken"].toString();
    QString refreshToken = jsonRsp["refreshToken"].toString();
    QString expiresIn = jsonRsp["expiresIn"].toString();

    using namespace std::chrono;
    seconds expiresSeconds = seconds(expiresIn.toInt());
    time_point<system_clock> now = system_clock::now();
    time_point<system_clock> expires = now + expiresSeconds;
    LOGD(QString("Now: %1").arg(now.time_since_epoch().count()));
    LOGD(QString("Expires: %1").arg(expires.time_since_epoch().count()));

    qlonglong expiresEpoch = expires.time_since_epoch().count();
    APP_SETTINGS->setUserSettings(userId, userToken, refreshToken, expiresEpoch);
    emit sgnUserCreated();
}
