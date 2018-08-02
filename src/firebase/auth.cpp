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
#define RECOVER_PASSWORD_URL "https://www.googleapis.com/identitytoolkit/v3/relyingparty/getOobConfirmationCode"
#define REFRESH_TOKEN_URL "https://securetoken.googleapis.com/v1/token"

FirebaseAuth::FirebaseAuth(QObject *parent) : QObject(parent)
{

}

void FirebaseAuth::signInUser(QString email, QString password)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("password", QJsonValue(password));
    jsonObj.insert("returnSecureToken", QJsonValue(true));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(SIGNIN_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &FirebaseAuth::authRequestOnFinish);
}

void FirebaseAuth::registerUser(QString email, QString password)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("password", QJsonValue(password));
    jsonObj.insert("returnSecureToken", QJsonValue(true));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(REGISTER_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &FirebaseAuth::authRequestOnFinish);
}

void FirebaseAuth::authRequestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    }
    emit sgnRequestFinished();

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

    bool fromSignUp = jsonRsp["kind"].toString() == "identitytoolkit#SignupNewUserResponse";
    if (fromSignUp) {
        ARENA_TRACKER->showMessage(tr("Signin Success."));
    };
    LOGD(QString("%1").arg(fromSignUp ? "User created" : "User signed"));

    UserSettings userSettings = createUserSettingsFromSign(jsonRsp);
    QString email = jsonRsp["email"].toString();
    QString userName = email.left(email.indexOf("@"));
    APP_SETTINGS->setUserSettings(userSettings, userName);
    emit sgnUserLogged(fromSignUp);
}

UserSettings FirebaseAuth::createUserSettingsFromSign(QJsonObject jsonRsp)
{
    QString userId = jsonRsp["localId"].toString();
    QString userToken = jsonRsp["idToken"].toString();
    QString refreshToken = jsonRsp["refreshToken"].toString();
    QString expiresIn = jsonRsp["expiresIn"].toString();
    return UserSettings(userId, userToken, refreshToken, getExpiresEpoch(expiresIn));
}

UserSettings FirebaseAuth::createUserSettingsFromRefreshedToken(QJsonObject jsonRsp)
{
    QString userId = jsonRsp["user_id"].toString();
    QString userToken = jsonRsp["id_token"].toString();
    QString refreshToken = jsonRsp["refresh_token"].toString();
    QString expiresIn = jsonRsp["expires_in"].toString();
    return UserSettings(userId, userToken, refreshToken, getExpiresEpoch(expiresIn));
}

qlonglong FirebaseAuth::getExpiresEpoch(QString expiresIn)
{
    using namespace std::chrono;
    seconds expiresSeconds = seconds(expiresIn.toInt());
    time_point<system_clock> now = system_clock::now();
    time_point<system_clock> expires = now + expiresSeconds;
    return expires.time_since_epoch().count();
}

void FirebaseAuth::refreshToken(QString refreshToken)
{
    QJsonObject jsonObj;
    jsonObj.insert("grant_type", "refresh_token");
    jsonObj.insert("refresh_token", QJsonValue(refreshToken));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(REFRESH_TOKEN_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &FirebaseAuth::tokenRefreshRequestOnFinish);
}

void FirebaseAuth::tokenRefreshRequestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    }
    emit sgnRequestFinished();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1").arg(reason));
        emit sgnTokenRefreshError();
        return;
    }
    LOGD(QString("%1").arg("UserToken refreshed"));

    UserSettings userSettings = createUserSettingsFromRefreshedToken(jsonRsp);
    APP_SETTINGS->setUserSettings(userSettings);
    emit sgnTokenRefreshed();
}

void FirebaseAuth::recoverPassword(QString email)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("requestType", "PASSWORD_RESET");
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(RECOVER_PASSWORD_URL);
    url.setQuery(QUrlQuery(QString("key=%1").arg(ApiKeys::FIREBASE())));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished, this, &FirebaseAuth::recoverPasswordRequestOnFinish);
}

void FirebaseAuth::recoverPasswordRequestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    }
    emit sgnRequestFinished();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1").arg(reason));
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
        if (message == "EMAIL_NOT_FOUND") {
            ARENA_TRACKER->showMessage(tr("Email not found."));
        }
        return;
    }
    LOGD(QString("%1").arg("Password recovered"));
    emit sgnPasswordRecovered();
}
