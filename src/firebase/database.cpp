#include "database.h"
#include "macros.h"
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

#define ARENA_META_DB_URL "https://firestore.googleapis.com/v1beta1/projects/arenameta-3b1a7/databases/(default)/documents"

FirebaseDatabase::FirebaseDatabase(QObject *parent, FirebaseAuth *firebaseAuth)
    : recallUpdatePlayerCollection(false), recallUpdateUserInventory(false),
      recallUpdatePlayerDeck(false), recalRegisterPlayerMatch(false)
{
    UNUSED(parent);
    this->firebaseAuth = firebaseAuth;
    connect(firebaseAuth, &FirebaseAuth::sgnTokenRefreshed,
            this, &FirebaseDatabase::onTokenRefreshed);
}

FirebaseDatabase::~FirebaseDatabase()
{
    DEL(firebaseAuth)
}

void FirebaseDatabase::onTokenRefreshed()
{
    if (recallUpdatePlayerCollection) {
        updatePlayerCollection(paramOwnedCards);
    }
    if (recallUpdateUserInventory) {
        updateUserInventory(paramPlayerInventory);
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

    RqtUpdatePlayerDeck rqtUpdatePlayerDeck(userSettings.userId, deck);
    sendPatchRequest(rqtUpdatePlayerDeck, userSettings.userToken);
}

void FirebaseDatabase::sendPatchRequest(FirestoreRequest firestoreRequest, QString userToken)
{
    QUrl url(QString("%1/%2").arg(ARENA_META_DB_URL).arg(firestoreRequest.path()));
    QByteArray bodyJson = firestoreRequest.body().toJson();
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
        LOGD(QString("Body: %1").arg(QString(bodyJson)));
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
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
        ARENA_TRACKER->showMessage(message);
        return;
    }

    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Database updated: %1").arg(reply->request().url().url()));
    }
}

void FirebaseDatabase::uploadMatch(MatchInfo matchInfo, QString playerRankClass,
                                   Deck playerDeck, Deck opponentDeck)
{
    rqtRegisterPlayerMatch = RqtRegisterPlayerMatch(matchInfo, playerDeck, opponentDeck);
    RqtUploadMatch rqtUploadMatch(matchInfo, playerRankClass, playerDeck, opponentDeck);
    QUrl url(QString("%1/%2").arg(ARENA_META_DB_URL).arg(rqtUploadMatch.path()));
    QByteArray bodyJson = rqtUploadMatch.body().toJson();

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
        LOGD(QString("Body: %1").arg(QString(bodyJson)));
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
        QJsonArray errors = jsonRsp["error"].toObject()["errors"].toArray();
        QString message = errors.first()["message"].toString();
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
