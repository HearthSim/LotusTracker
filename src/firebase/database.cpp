#include "database.h"
#include "macros.h"
#include "../apikeys.h"
#include "rqtcreateplayerdeck.h"
#include "rqtcreateplayerdeckupdate.h"
#include "rqtupdateplayercollection.h"
#include "rqtupdateplayerdeck.h"
#include "rqtupdateuserinventory.h"
#include "rqtuploadmatch.h"

#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#define FIREBASE_PREFIX "https://firestore.googleapis.com/v1beta1/projects"
#define FIREBASE_SUFIX "databases/(default)/documents"

FirebaseDatabase::FirebaseDatabase(QObject *parent, FirebaseAuth *firebaseAuth)
    : recallUpdatePlayerCollection(false), recallUpdateUserInventory(false),
      recallGetPlayerDeck(false), recallCreatePlayerDeck(false),
      recallUpdatePlayerDeck(false), recalRegisterPlayerMatch(false)
{
    UNUSED(parent);
    this->firebaseAuth = firebaseAuth;
    firebaseDBUrl = QString("%1/%2/%3").arg(FIREBASE_PREFIX)
            .arg(ApiKeys::FIREBASE_PROJECT()).arg(FIREBASE_SUFIX);
    connect(firebaseAuth, &FirebaseAuth::sgnTokenRefreshed,
            this, &FirebaseDatabase::onTokenRefreshed);
}

FirebaseDatabase::~FirebaseDatabase()
{

}

void FirebaseDatabase::onTokenRefreshed()
{
    if (recallUpdatePlayerCollection) {
        updatePlayerCollection(paramOwnedCards);
    }
    if (recallUpdateUserInventory) {
        updateUserInventory(paramPlayerInventory);
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

void FirebaseDatabase::updatePlayerCollection(QMap<int, int> ownedCards)
{
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
    RqtUpdatePlayerCollection rqtUpdatePlayerCollection(userSettings.userId, ownedCards);
    sendPatchRequest(rqtUpdatePlayerCollection, userSettings.userToken);
}

void FirebaseDatabase::updateUserInventory(PlayerInventory playerInventory)
{
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
    RqtUpdateUserInventory rqtUpdateUserInventory(userSettings.userId, playerInventory);
    sendPatchRequest(rqtUpdateUserInventory, userSettings.userToken);
}

void FirebaseDatabase::createPlayerDeck(Deck deck)
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

    RqtCreatePlayerDeck rqtCreatePlayerDeck(userSettings.userId, deck);
    sendPatchRequest(rqtCreatePlayerDeck, userSettings.userToken);
}

void FirebaseDatabase::updatePlayerDeck(Deck deck)
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

void FirebaseDatabase::getPlayerDeckToUpdate(QString deckID)
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

    QUrl url(QString("%1/users/GI29TOaqwAhyW99JuT4Xh1QCXkS2/decks/%2").arg(firebaseDBUrl)
             .arg(deckID));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString("Authorization").toUtf8(),
                         QString("Bearer %1").arg(userSettings.userToken).toUtf8());
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Get Request: %1").arg(url.toString()));
    }
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished,
            this, &FirebaseDatabase::getPlayerDeckToUpdateRequestOnFinish);
}

void FirebaseDatabase::getPlayerDeckToUpdateRequestOnFinish()
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
        QJsonObject error = jsonRsp["error"].toObject();
        QString status = error["status"].toString();
        if (status == "NOT_FOUND") {
            createPlayerDeck(paramDeck);
            return;
        }
        QString message = error["message"].toString();
        ARENA_TRACKER->showMessage(message);
        return;
    }

    Deck oldDeck = firestoreJsonToDeck(jsonRsp);
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    //Update deck data
    RqtUpdatePlayerDeck rqtUpdatePlayerDeck(userSettings.userId, paramDeck);
    sendPatchRequest(rqtUpdatePlayerDeck, userSettings.userToken);
    //Create deck update
    RqtCreatePlayerDeckUpdate rqtCreatePlayerDeckUpdate(userSettings.userId, paramDeck, oldDeck);
    sendPatchRequest(rqtCreatePlayerDeckUpdate, userSettings.userToken);
}

Deck FirebaseDatabase::firestoreJsonToDeck(QJsonObject deckJson)
{
    QString friebaseName = deckJson["name"].toString();
    QString id = friebaseName.right(friebaseName.size() - friebaseName.lastIndexOf("/") - 1);
    QString name = deckJson["fields"].toObject()["name"].toObject()["stringValue"].toString();
    QJsonObject cardsFields = deckJson["fields"].toObject()["cards"]
            .toObject()["mapValue"].toObject()["fields"].toObject();
    QMap<Card*, int> cards;
    for (QString key : cardsFields.keys()) {
        Card* card = ARENA_TRACKER->mtgCards->findCard(key.toInt());
        cards[card] = cardsFields[key].toObject()["integerValue"].toString().toInt();
    }
    QJsonObject sideboardCardsFields = deckJson["fields"].toObject()["sideboard"]
            .toObject()["mapValue"].toObject()["fields"].toObject();
    QMap<Card*, int> sideboard;
    for (QString key : sideboardCardsFields.keys()) {
        Card* card = ARENA_TRACKER->mtgCards->findCard(key.toInt());
        sideboard[card] = sideboardCardsFields[key].toObject()["integerValue"].toString().toInt();
    }
    return Deck(id, name, cards, sideboard);
}

void FirebaseDatabase::sendPatchRequest(FirestoreRequest firestoreRequest, QString userToken)
{
    QUrl url(QString("%1/%2").arg(firebaseDBUrl).arg(firestoreRequest.path()),
             firestoreRequest.hasDuplicateQuery() ? QUrl::StrictMode : QUrl::TolerantMode);
    QByteArray bodyJson = firestoreRequest.body().toJson();
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Patch Request: %1").arg(url.toString()));
        LOGD(QString("Patch Body: %1").arg(QString(bodyJson)));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString("Authorization").toUtf8(),
                         QString("Bearer %1").arg(userToken).toUtf8());
    QBuffer *buffer = new QBuffer();
    buffer->open((QBuffer::ReadWrite));
    buffer->write(bodyJson);
    buffer->seek(0);
    QNetworkReply *reply = networkManager.sendCustomRequest(request, "PATCH", buffer);
    connect(reply, &QNetworkReply::finished,
            this, &FirebaseDatabase::requestOnFinish);
}

void FirebaseDatabase::requestOnFinish()
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
        QJsonObject error = jsonRsp["error"].toObject();
        QString message = error["message"].toString();
        ARENA_TRACKER->showMessage(message);
        return;
    }

    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Database updated: %1").arg(reply->request().url().url()));
    }
}

void FirebaseDatabase::uploadMatch(MatchInfo matchInfo, Deck playerDeck,
                                   QString playerRankClass)
{
    rqtRegisterPlayerMatch = RqtRegisterPlayerMatch(matchInfo, playerDeck);
    RqtUploadMatch rqtUploadMatch(matchInfo, playerDeck, playerRankClass);
    QUrl url(QString("%1/%2").arg(firebaseDBUrl).arg(rqtUploadMatch.path()));
    QByteArray bodyJson = rqtUploadMatch.body().toJson();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Post Request: %1").arg(url.toString()));
        LOGD(QString("Post Body: %1").arg(QString(bodyJson)));
    }
    QNetworkReply *reply = networkManager.post(request, bodyJson);
    connect(reply, &QNetworkReply::finished,
            this, &FirebaseDatabase::uploadMatchRequestOnFinish);
}

void FirebaseDatabase::uploadMatchRequestOnFinish()
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
        QJsonObject error = jsonRsp["error"].toObject();
        QString message = error["message"].toString();
        ARENA_TRACKER->showMessage(message);
        return;
    }

    LOGD(QString("Match uploaded anonymously"));
    QString name = jsonRsp["name"].toString();
    QString matchID = name.right(name.size() - name.lastIndexOf("/") - 1);
    registerPlayerMatch(matchID);
}

void FirebaseDatabase::registerPlayerMatch(QString matchID)
{
    recalRegisterPlayerMatch = false;
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    if (userSettings.userToken.isEmpty() || rqtRegisterPlayerMatch.path().isEmpty()) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramMatchID = matchID;
        recalRegisterPlayerMatch = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }

    rqtRegisterPlayerMatch.createPath(userSettings.userId, matchID);
    sendPatchRequest(rqtRegisterPlayerMatch, userSettings.userToken);
}
