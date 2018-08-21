#include "database.h"
#include "macros.h"
#include "../apikeys.h"
#include "rqtplayerdeck.h"
#include "rqtplayerdeckupdate.h"
#include "rqtupdateplayercollection.h"
#include "rqtupdateplayerinventory.h"
#include "rqtuploadmatch.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

LotusTrackerAPI::LotusTrackerAPI(QObject *parent, FirebaseAuth *firebaseAuth)
    : recallUpdatePlayerCollection(false), recallUpdateUserInventory(false),
      recallGetPlayerDeck(false), recallCreatePlayerDeck(false),
      recallUpdatePlayerDeck(false), recalRegisterPlayerMatch(false)
{
    UNUSED(parent);
    this->firebaseAuth = firebaseAuth;
    connect(firebaseAuth, &FirebaseAuth::sgnTokenRefreshed,
            this, &LotusTrackerAPI::onTokenRefreshed);
}

LotusTrackerAPI::~LotusTrackerAPI()
{

}

void LotusTrackerAPI::onTokenRefreshed()
{
    if (recallUpdatePlayerCollection) {
        updatePlayerCollection(paramOwnedCards);
    }
    if (recallUpdateUserInventory) {
        updatePlayerInventory(paramPlayerInventory);
    }
    if (recallGetPlayerDeck) {
        getPlayerDeckToUpdate(paramDeckID);
    }
    if (recallCreatePlayerDeck) {
        createPlayerDeck(paramDeck);
    }
    if (recallUpdatePlayerDeck) {
        updatePlayerDeck(paramDeck);
    }
    if (recalRegisterPlayerMatch) {
        registerPlayerMatch(paramMatchID);
    }
}

void LotusTrackerAPI::updatePlayerCollection(QMap<int, int> ownedCards)
{
    QDateTime now = QDateTime::currentDateTime();
    if (!lastUpdatePlayerCollectionDate.isNull() &&
            lastUpdatePlayerCollectionDate.secsTo(now) <= 30*60) {
        return;
    }
    lastUpdatePlayerCollectionDate = now;
    recallUpdatePlayerCollection = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramOwnedCards = ownedCards;
        recallUpdatePlayerCollection = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }
    sendPost(RqtUpdatePlayerCollection(userSettings.userId, ownedCards));
}

void LotusTrackerAPI::updatePlayerInventory(PlayerInventory playerInventory)
{
    QDateTime now = QDateTime::currentDateTime();
    if (!lastUpdatePlayerInventoryDate.isNull() &&
            lastUpdatePlayerInventoryDate.secsTo(now) <= 30*60) {
        return;
    }
    lastUpdatePlayerInventoryDate = now;
    recallUpdateUserInventory = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramPlayerInventory = playerInventory;
        recallUpdateUserInventory = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }
    sendPatch(RqtUpdatePlayerInventory(userSettings.userId, playerInventory));
}

void LotusTrackerAPI::createPlayerDeck(Deck deck)
{
    recallCreatePlayerDeck = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramDeck = deck;
        recallCreatePlayerDeck = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }
    sendPost(RqtPlayerDeck(userSettings.userId, deck));
}

void LotusTrackerAPI::updatePlayerDeck(Deck deck)
{
    recallUpdatePlayerDeck = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramDeck = deck;
        recallUpdatePlayerDeck = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }
    paramDeck = deck;
    getPlayerDeckToUpdate(deck.id);
}

void LotusTrackerAPI::getPlayerDeckToUpdate(QString deckID)
{
    recallGetPlayerDeck = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramDeckID = deckID;
        recallGetPlayerDeck = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }

    QUrl url(QString("%1/users/decks?userId=%2&deckId=%3").arg(ApiKeys::API_BASE_URL())
             .arg(userSettings.userId).arg(deckID));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString("Authorization").toUtf8(),
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
        if (statusCode == 404) {
            createPlayerDeck(paramDeck);
            return;
        }
        QString message = jsonRsp["error"].toString();
        ARENA_TRACKER->showMessage(message);
        return;
    }

    Deck oldDeck = jsonToDeck(jsonRsp);
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    //Update deck data
    sendPatch(RqtPlayerDeck(userSettings.userId, paramDeck));
    //Create deck update
    sendPost(RqtPlayerDeckUpdate(userSettings.userId, paramDeck, oldDeck));
}

Deck LotusTrackerAPI::jsonToDeck(QJsonObject deckJson)
{
    QString id = deckJson["id"].toString();
    QString name = deckJson["name"].toString();
    QJsonObject cardsFields = deckJson["cards"].toObject();
    QMap<Card*, int> cards;
    for (QString key : cardsFields.keys()) {
        Card* card = ARENA_TRACKER->mtgCards->findCard(key.toInt());
        cards[card] = cardsFields[key].toInt();
    }
    QJsonObject sideboardCardsFields = deckJson["sideboard"].toObject();
    QMap<Card*, int> sideboard;
    for (QString key : sideboardCardsFields.keys()) {
        Card* card = ARENA_TRACKER->mtgCards->findCard(key.toInt());
        sideboard[card] = sideboardCardsFields[key].toInt();
    }
    return Deck(id, name, cards, sideboard);
}

QNetworkRequest LotusTrackerAPI::prepareRequest(RequestData firestoreRequest)
{
    QUrl url(QString("%1/%2").arg(ApiKeys::API_BASE_URL()).arg(firestoreRequest.path()));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QString userToken = APP_SETTINGS->getUserSettings().userToken;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString("Authorization").toUtf8(),
                         QString("Bearer %1").arg(userToken).toUtf8());
    return request;
}

QBuffer* LotusTrackerAPI::prepareBody(RequestData firestoreRequest)
{
    QByteArray bodyJson = firestoreRequest.body().toJson();
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
    QNetworkRequest request = prepareRequest(requestData);
    QBuffer* buffer = prepareBody(requestData);
    QNetworkReply *reply = networkManager.sendCustomRequest(request, "PATCH", buffer);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::requestOnFinish);
}

void LotusTrackerAPI::sendPost(RequestData requestData)
{
    QNetworkRequest request = prepareRequest(requestData);
    QBuffer* buffer = prepareBody(requestData);
    QNetworkReply *reply = networkManager.post(request, buffer);
    connect(reply, &QNetworkReply::finished,
            this, &LotusTrackerAPI::requestOnFinish);
}

void LotusTrackerAPI::requestOnFinish()
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
        ARENA_TRACKER->showMessage(error);
        return;
    }

    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Database updated"));
    }
}

void LotusTrackerAPI::uploadMatch(MatchInfo matchInfo, Deck playerDeck,
                                   QString playerRankClass)
{
    rqtRegisterPlayerMatch = RqtRegisterPlayerMatch(matchInfo, playerDeck);
    RqtUploadMatch rqtUploadMatch(matchInfo, playerDeck, playerRankClass);
    QNetworkRequest request = prepareRequest(rqtUploadMatch);
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
        ARENA_TRACKER->showMessage(error);
        return;
    }

    LOGD(QString("Match uploaded anonymously"));
    QString matchID = jsonRsp["id"].toString();
    registerPlayerMatch(matchID);
}

void LotusTrackerAPI::registerPlayerMatch(QString matchID)
{
    recalRegisterPlayerMatch = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramMatchID = matchID;
        recalRegisterPlayerMatch = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }

    rqtRegisterPlayerMatch.createPath(userSettings.userId, matchID);
    sendPost(rqtRegisterPlayerMatch);
}
