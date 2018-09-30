#include "lotusapi.h"
#include "macros.h"
#include "../urls.h"
#include "rqtplayerdeck.h"
#include "rqtplayerdeckpublish.h"
#include "rqtplayerdeckupdate.h"
#include "rqtupdateplayercollection.h"
#include "rqtupdateplayerinventory.h"
#include "rqtuploadmatch.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#define HEADER_AUTHORIZATION "Authorization"
#define UPDATE_DELAY 30 //minutes between each player inventory and collection update

LotusTrackerAPI::LotusTrackerAPI(QObject *parent)
{
    UNUSED(parent);
    isRefreshTokenInProgress = false;
    connect(this, &LotusTrackerAPI::sgnTokenRefreshed,
            this, &LotusTrackerAPI::onTokenRefreshed);
}

LotusTrackerAPI::~LotusTrackerAPI()
{

}

void LotusTrackerAPI::signInUser(QString email, QString password)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("password", QJsonValue(password));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(QString("%1/signin").arg(URLs::API()));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::authRequestOnFinish);
}

void LotusTrackerAPI::registerUser(QString email, QString password)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    jsonObj.insert("password", QJsonValue(password));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(QString("%1/signup").arg(URLs::API()));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::authRequestOnFinish);
}

void LotusTrackerAPI::authRequestOnFinish()
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
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
        if (message.contains("EMAIL_EXISTS")) {
            LOTUS_TRACKER->showMessage(tr("Email already in use. Try do login."));
        } else if (message.contains("INVALID_PASSWORD")) {
            LOTUS_TRACKER->showMessage(tr("Invalid password."));
        } else if (message.contains("WEAK_PASSWORD")) {
            LOTUS_TRACKER->showMessage(tr("Password should be at least 6 characters."));
        } else {
            LOTUS_TRACKER->showMessage(message);
        }
        return;
    }

    bool fromSignUp = jsonRsp["kind"].toString() == "identitytoolkit#SignupNewUserResponse";
    if (fromSignUp) {
        LOTUS_TRACKER->showMessage(tr("Signin Success."));
    };
    LOGD(QString("%1").arg(fromSignUp ? "User created" : "User signed"));

    UserSettings userSettings = createUserSettingsFromSign(jsonRsp);
    QString email = jsonRsp["email"].toString();
    QString userName = email.left(email.indexOf("@"));
    APP_SETTINGS->setUserSettings(userSettings, userName);
    emit sgnUserLogged(fromSignUp);
}

UserSettings LotusTrackerAPI::createUserSettingsFromSign(QJsonObject jsonRsp)
{
    QString userId = jsonRsp["localId"].toString();
    QString userToken = jsonRsp["idToken"].toString();
    QString refreshToken = jsonRsp["refreshToken"].toString();
    QString expiresIn = jsonRsp["expiresIn"].toString();
    return UserSettings(userId, userToken, refreshToken, getExpiresEpoch(expiresIn));
}

void LotusTrackerAPI::refreshToken(QString refreshToken)
{
    if (isRefreshTokenInProgress) {
        return;
    }
    isRefreshTokenInProgress = true;
    QJsonObject jsonObj;
    jsonObj.insert("refresh_token", QJsonValue(refreshToken));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(QString("%1/refreshtoken").arg(URLs::API()));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::tokenRefreshRequestOnFinish);
}

void LotusTrackerAPI::tokenRefreshRequestOnFinish()
{
    isRefreshTokenInProgress = false;
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    }
    emit sgnRequestFinished();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        emit sgnTokenRefreshError();
        return;
    }
    LOGD(QString("%1").arg("UserToken refreshed"));

    UserSettings userSettings = createUserSettingsFromRefreshedToken(jsonRsp);
    APP_SETTINGS->setUserSettings(userSettings);
    emit sgnTokenRefreshed();
}

UserSettings LotusTrackerAPI::createUserSettingsFromRefreshedToken(QJsonObject jsonRsp)
{
    QString userId = jsonRsp["user_id"].toString();
    QString userToken = jsonRsp["id_token"].toString();
    QString refreshToken = jsonRsp["refresh_token"].toString();
    QString expiresIn = jsonRsp["expires_in"].toString();
    return UserSettings(userId, userToken, refreshToken, getExpiresEpoch(expiresIn));
}

qlonglong LotusTrackerAPI::getExpiresEpoch(QString expiresIn)
{
    using namespace std::chrono;
    seconds expiresSeconds = seconds(expiresIn.toInt());
    time_point<system_clock> now = system_clock::now();
    time_point<system_clock> expires = now + expiresSeconds;
    return expires.time_since_epoch().count();
}

void LotusTrackerAPI::onTokenRefreshed()
{
    for(QString requestUrl : requestsToRecall.keys()) {
        QPair<QString, RequestData> methodRequest = requestsToRecall[requestUrl];
        LOGD(QString("Recalling request"));
        if (methodRequest.first == "POST") {
            sendPost(methodRequest.second);
        }
        if (methodRequest.first == "PATCH") {
            sendPatch(methodRequest.second);
        }
    }
}

void LotusTrackerAPI::recoverPassword(QString email)
{
    QJsonObject jsonObj;
    jsonObj.insert("email", QJsonValue(email));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(QString("%1/recoverpassword").arg(URLs::API()));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::recoverPasswordRequestOnFinish);
}

void LotusTrackerAPI::recoverPasswordRequestOnFinish()
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
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
        if (message == "EMAIL_NOT_FOUND") {
            LOTUS_TRACKER->showMessage(tr("Email not found."));
        } else {
            LOTUS_TRACKER->showMessage(message);
        }
        return;
    }
    LOGD(QString("%1").arg("Password recovered"));
    emit sgnPasswordRecovered();
}

void LotusTrackerAPI::updatePlayerCollection(QMap<int, int> ownedCards)
{
    QDateTime now = QDateTime::currentDateTime();
    if (!lastUpdatePlayerCollectionDate.isNull() &&
            lastUpdatePlayerCollectionDate.secsTo(now) <= UPDATE_DELAY*60) {
        return;
    }
    lastUpdatePlayerCollectionDate = now;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    sendPost(RqtUpdatePlayerCollection(userSettings.userId, ownedCards));
}

void LotusTrackerAPI::updatePlayerInventory(PlayerInventory playerInventory)
{
    QDateTime now = QDateTime::currentDateTime();
    if (!lastUpdatePlayerInventoryDate.isNull() &&
            lastUpdatePlayerInventoryDate.secsTo(now) <= UPDATE_DELAY*60) {
        return;
    }
    lastUpdatePlayerInventoryDate = now;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    sendPatch(RqtUpdatePlayerInventory(userSettings.userId, playerInventory));
}

void LotusTrackerAPI::createPlayerDeck(Deck deck)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    sendPost(RqtPlayerDeck(userSettings.userId, deck));
}

void LotusTrackerAPI::updatePlayerDeck(Deck deck)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    paramDeck = deck;
    getPlayerDeckToUpdate(deck.id);
}

void LotusTrackerAPI::publishOrUpdatePlayerDeck(QString playerName, Deck deck)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    sendPost(RqtPublishPlayerDeck(playerName, deck));
}

void LotusTrackerAPI::getPlayerDeckWinRate(QString deckId, QString eventId)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    QUrl url(QString("%1/users/decks/winrate?userId=%2&deckId=%3&eventId=%4")
             .arg(URLs::API()).arg(userSettings.userId).arg(deckId).arg(eventId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString(HEADER_AUTHORIZATION).toUtf8(),
                         QString("Bearer %1").arg(userSettings.userToken).toUtf8());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Get Request: %1").arg(url.toString()));
    }
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::getPlayerDeckWinRateRequestOnFinish);
}

void LotusTrackerAPI::getPlayerDeckWinRateRequestOnFinish()
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
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        if (statusCode == 404) {    //Not found
            return;
        }
        QString message = jsonRsp["error"].toString();
        LOTUS_TRACKER->showMessage(message);
        return;
    }

    int wins = jsonRsp["wins"].toInt();
    int losses = jsonRsp["losses"].toInt();
    double winRate = jsonRsp["winrate"].toDouble();
    emit sgnDeckWinRate(wins, losses, winRate);
}

void LotusTrackerAPI::getPlayerDeckToUpdate(QString deckID)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }

    QUrl url(QString("%1/users/decks?userId=%2&deckId=%3").arg(URLs::API())
             .arg(userSettings.userId).arg(deckID));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString(HEADER_AUTHORIZATION).toUtf8(),
                         QString("Bearer %1").arg(userSettings.userToken).toUtf8());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Get Request: %1").arg(url.toString()));
    }
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::getPlayerDeckToUpdateRequestOnFinish);
}

void LotusTrackerAPI::getPlayerDeckToUpdateRequestOnFinish()
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
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        if (statusCode == 404) {    //Not found
            createPlayerDeck(paramDeck);
            return;
        }
        QString message = jsonRsp["error"].toString();
        LOTUS_TRACKER->showMessage(message);
        return;
    }

    Deck oldDeck = jsonToDeck(jsonRsp);
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    //Update deck data
    sendPatch(RqtPlayerDeck(userSettings.userId, paramDeck));
    //Create deck update
    RqtPlayerDeckUpdate rqtPlayerDeckUpdate(userSettings.userId, paramDeck, oldDeck);
    if (rqtPlayerDeckUpdate.isValid()) {
        sendPost(rqtPlayerDeckUpdate);
    }
}

Deck LotusTrackerAPI::jsonToDeck(QJsonObject deckJson)
{
    QString id = deckJson["id"].toString();
    QString name = deckJson["name"].toString();
    QJsonObject cardsFields = deckJson["cards"].toObject();
    QMap<Card*, int> cards;
    for (QString key : cardsFields.keys()) {
        Card* card = LOTUS_TRACKER->mtgCards->findCard(key.toInt());
        cards[card] = cardsFields[key].toInt();
    }
    QJsonObject sideboardCardsFields = deckJson["sideboard"].toObject();
    QMap<Card*, int> sideboard;
    for (QString key : sideboardCardsFields.keys()) {
        Card* card = LOTUS_TRACKER->mtgCards->findCard(key.toInt());
        sideboard[card] = sideboardCardsFields[key].toInt();
    }
    return Deck(id, name, cards, sideboard);
}

void LotusTrackerAPI::uploadMatch(MatchInfo matchInfo, Deck playerDeck,
                                   QString playerRankClass)
{
    rqtRegisterPlayerMatch = RqtRegisterPlayerMatch(matchInfo, playerDeck);
    RqtUploadMatch rqtUploadMatch(matchInfo, playerDeck, playerRankClass);
    QNetworkRequest request = prepareRequest(rqtUploadMatch, false);
    QBuffer* buffer = prepareBody(rqtUploadMatch);
    QNetworkReply *reply = networkManager.post(request, buffer);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::uploadMatchRequestOnFinish);
}

void LotusTrackerAPI::uploadMatchRequestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request response: %1").arg(reply->request().url().url()));
        LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    }
    emit sgnRequestFinished();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        QString error = jsonRsp["error"].toString();
        LOTUS_TRACKER->showMessage(error);
        return;
    }

    LOGD(QString("Match uploaded anonymously"));
    QString matchID = jsonRsp["id"].toString();
    registerPlayerMatch(matchID);
}

void LotusTrackerAPI::registerPlayerMatch(QString matchID)
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }

    rqtRegisterPlayerMatch.createPath(userSettings.userId, matchID);
    sendPost(rqtRegisterPlayerMatch);
}

QNetworkRequest LotusTrackerAPI::prepareRequest(RequestData requestData,
                                                bool checkUserAuth, QString method)
{
    QUrl url(QString("%1/%2").arg(URLs::API()).arg(requestData.path()));
    QNetworkRequest request(url);

    requestsToRecall[url.toString()] = qMakePair(method, requestData);
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (checkUserAuth && !userSettings.isAuthValid()) {
        LOGD(QString("User token expired.").arg(url.toString()));
        refreshToken(userSettings.refreshToken);
        return request;
    }

    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QString userToken = APP_SETTINGS->getUserSettings().userToken;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString(HEADER_AUTHORIZATION).toUtf8(),
                         QString("Bearer %1").arg(userToken).toUtf8());
    return request;
}

QBuffer* LotusTrackerAPI::prepareBody(RequestData requestData)
{
    QByteArray bodyJson = requestData.body().toJson();
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Body: %1").arg(QString(bodyJson)));
    }
    QBuffer* buffer = new QBuffer();
    buffer->open((QBuffer::ReadWrite));
    buffer->write(bodyJson);
    buffer->seek(0);
    return buffer;
}

void LotusTrackerAPI::sendPatch(RequestData requestData)
{
    QNetworkRequest request = prepareRequest(requestData, true, "PATCH");
    if (!request.hasRawHeader(QString(HEADER_AUTHORIZATION).toUtf8())) {
        return;
    }
    QBuffer* buffer = prepareBody(requestData);
    QNetworkReply *reply = networkManager.sendCustomRequest(request, "PATCH", buffer);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::requestOnFinish);
}

void LotusTrackerAPI::sendPost(RequestData requestData)
{
    QNetworkRequest request = prepareRequest(requestData, true, "POST");
    if (!request.hasRawHeader(QString(HEADER_AUTHORIZATION).toUtf8())) {
        return;
    }
    QBuffer* buffer = prepareBody(requestData);
    QNetworkReply *reply = networkManager.post(request, buffer);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::requestOnFinish);
}

void LotusTrackerAPI::requestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QString requestUrl = reply->request().url().toString();
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request response: %1").arg(requestUrl));
        LOGD(QString(QJsonDocument(jsonRsp).toJson()));
    }
    emit sgnRequestFinished();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        if (statusCode == 401) {    //Token expired
            UserSettings userSettings = APP_SETTINGS->getUserSettings();
            refreshToken(userSettings.refreshToken);
            return;
        }
        QString error = jsonRsp["error"].toString();
        if (error.isEmpty()) {
            LOTUS_TRACKER->showMessage(reason);
        } else {
            LOTUS_TRACKER->showMessage(error);
        }
        emit sgnRequestFinishedWithError();
        return;
    }
    emit sgnRequestFinishedWithSuccess();

    requestsToRecall.remove(requestUrl);
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Database updated"));
    }
}
