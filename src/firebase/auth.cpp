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
#define SIGNIN_URL "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword"
#define REFRESH_TOKEN_URL "https://securetoken.googleapis.com/v1/token"

Auth::Auth(QObject *parent) : QObject(parent)
{

}

void Auth::signInUser(QString email, QString password)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("password", QJsonValue(password));
    jsonObj.insert("returnSecureToken", QJsonValue(true));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(SIGNIN_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    LOGD(QString("Request: %1").arg(url.toString()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &Auth::authRequestOnFinish);
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
    LOGD(QString("Request: %1").arg(url.toString()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &Auth::authRequestOnFinish);
}

void Auth::refreshToken(QString refreshToken)
{
    QJsonObject jsonObj;
    jsonObj.insert("grant_type", "refresh_token");
    jsonObj.insert("refresh_token", QJsonValue(refreshToken));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(REFRESH_TOKEN_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    LOGD(QString("Request: %1").arg(url.toString()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &Auth::authRequestOnFinish);
}

void Auth::authRequestOnFinish()
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
        if (message == "INVALID_PASSWORD") {
            ARENA_TRACKER->showMessage(tr("Invalid password."));
        }
        return;
    }

    bool signRequest = jsonRsp.contains("kind");
    bool fromSignUp = jsonRsp["kind"].toString() == "identitytoolkit#SignupNewUserResponse";
    if (fromSignUp) {
        ARENA_TRACKER->showMessage(tr("Signin Success."));
    };
    LOGD(QString("%1").arg(!signRequest ? "Token refreshed" :
                                          fromSignUp ? "User created" : "User signed"));

    APP_SETTINGS->setUserSettings(signRequest ? createUserSettingsFromSign(jsonRsp)
                                              : createUserSettingsFromRefreshedToken(jsonRsp));
    emit (signRequest ? sgnUserLogged(fromSignUp) : sgnTokenRefreshed());
}

UserSettings Auth::createUserSettingsFromSign(QJsonObject jsonRsp)
{
    QString userId = jsonRsp["localId"].toString();
    QString userToken = jsonRsp["idToken"].toString();
    QString refreshToken = jsonRsp["refreshToken"].toString();
    QString expiresIn = jsonRsp["expiresIn"].toString();
    return UserSettings(userId, userToken, refreshToken, getExpiresEpoch(expiresIn));
}

UserSettings Auth::createUserSettingsFromRefreshedToken(QJsonObject jsonRsp)
{
    QString userId = jsonRsp["user_id"].toString();
    QString userToken = jsonRsp["id_token"].toString();
    QString refreshToken = jsonRsp["refresh_token"].toString();
    QString expiresIn = jsonRsp["expires_in"].toString();
    return UserSettings(userId, userToken, refreshToken, getExpiresEpoch(expiresIn));
}

qlonglong Auth::getExpiresEpoch(QString expiresIn)
{
    using namespace std::chrono;
    seconds expiresSeconds = seconds(expiresIn.toInt());
    time_point<system_clock> now = system_clock::now();
    time_point<system_clock> expires = now + expiresSeconds;
    LOGD(QString("Now: %1").arg(now.time_since_epoch().count()));
    LOGD(QString("Expires: %1").arg(expires.time_since_epoch().count()));
    return expires.time_since_epoch().count();
}
