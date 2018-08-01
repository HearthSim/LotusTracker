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

FirebaseDatabase::FirebaseDatabase(QObject *parent, FirebaseAuth *firebaseAuth)
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
    QJsonObject jsonInventoryFields{
        { "wcCommon", QJsonObject{
                { "integerValue", QString::number(playerInventory.getWcCommon()) }}},
        { "wcUncommon", QJsonObject{
                { "integerValue", QString::number(playerInventory.getWcUncommon()) }}},
        { "wcRare", QJsonObject{
                { "integerValue", QString::number(playerInventory.getWcRare()) }}},
        { "wcMythic", QJsonObject{
                { "integerValue", QString::number(playerInventory.getWcMythic()) }}}
    };
    QJsonObject jsonObj{
        {"fields", QJsonObject{
                {"inventory", QJsonObject{
                        {"mapValue", QJsonObject{
                                {"fields", jsonInventoryFields}
                            }}
                    }}
            }}
    };
    QUrl url(QString("%1/users/%2?updateMask.fieldPaths=inventory")
             .arg(ARENA_META_DB_URL).arg(userSettings.userId));
    createPatchRequest(url, QJsonDocument(jsonObj), userSettings.userToken);
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

    QJsonObject jsonCards;
    QMap<Card*, int> cards = deck.cards();
    for (Card* card : cards.keys()) {
        jsonCards.insert(QString::number(card->mtgaId),
                         QJsonObject{{ "integerValue", QString::number(cards[card]) }});
    }
    QJsonObject jsonObj{
        {"fields", QJsonObject{
                { "cards", QJsonObject{
                        { "mapValue", QJsonObject{
                                {"fields", jsonCards}
                            }}}},
                { "colors", QJsonObject{
                        { "stringValue", deck.colorIdentity() }}},
                { "name", QJsonObject{
                        { "stringValue", deck.name }}}
            }}
    };
    QUrl url(QString("%1/users/%2/decks/%3").arg(ARENA_META_DB_URL)
             .arg(userSettings.userId).arg(deck.id));
    createPatchRequest(url, QJsonDocument(jsonObj), userSettings.userToken);
}

void FirebaseDatabase::createPatchRequest(QUrl url, QJsonDocument body, QString userToken)
{
#ifdef QT_DEBUG
    LOGD(QString("Request: %1").arg(url.toString()));
    LOGD(QString("Body: %1").arg(QString(body.toJson())));
#endif
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(QString("Authorization").toUtf8(),
                         QString("Bearer %1").arg(userToken).toUtf8());
    QBuffer *buffer = new QBuffer();
    buffer->open((QBuffer::ReadWrite));
    buffer->write(body.toJson());
    buffer->seek(0);
    QNetworkReply *reply = networkManager.sendCustomRequest(request, "PATCH", buffer);
    connect(reply, &QNetworkReply::finished,
            this, &FirebaseDatabase::requestOnFinish);
}

void FirebaseDatabase::requestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
#ifdef QT_DEBUG
    LOGD(QString(QJsonDocument(jsonRsp).toJson()));
#endif
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

#ifdef QT_DEBUG
    LOGD(QString("Database updated: %1").arg(reply->request().url().url()));
#endif
}

void FirebaseDatabase::uploadMatch(MatchInfo matchInfo, QString playerRankClass,
                                   Deck playerDeck, Deck opponentDeck)
{
    jsonPlayerMatchObj = QJsonObject{
        {"fields", QJsonObject{
                {"deck", QJsonObject{
                        {"stringValue", playerDeck.id}
                    }},
                {"event", QJsonObject{
                        {"stringValue", matchInfo.eventId}
                    }},
                {"opponentName", QJsonObject{
                        {"stringValue", matchInfo.opponentInfo.opponentName()}
                    }},
                {"opponentDeckColors", QJsonObject{
                        {"stringValue", opponentDeck.colorIdentity(false)}
                    }},
                {"wins", QJsonObject{
                        {"booleanValue", matchInfo.playerWins}
                    }}
            }}};

    QString first = matchInfo.playerGoFirst ? "player1" : "player2";
    QString winner = matchInfo.playerWins ? "player1" : "player2";
    QJsonObject jsonObj{
        {"fields", QJsonObject{
                {"event", QJsonObject{
                        {"stringValue", matchInfo.eventId}
                    }},
                {"first", QJsonObject{
                        {"stringValue", first}
                    }},
                {"player1", QJsonObject{
                        {"mapValue", toJsonFields(playerDeck, matchInfo,
                                                  true, playerRankClass)}
                    }},
                {"player2", QJsonObject{
                        {"mapValue", toJsonFields(opponentDeck, matchInfo, false) }
                    }},
                {"winner", QJsonObject{
                        {"stringValue", winner}
                    }}
            }}
    };


    QDate date = QDate::currentDate();
    QUrl url(QString("%1/matches/%2/%3").arg(ARENA_META_DB_URL)
             .arg(date.year()).arg(date.month()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonDocument body = QJsonDocument(jsonObj);

#ifdef QT_DEBUG
    LOGD(QString("Request: %1").arg(url.toString()));
    LOGD(QString("Body: %1").arg(QString(body.toJson())));
#endif

    QNetworkReply *reply = networkManager.post(request, body.toJson());
    connect(reply, &QNetworkReply::finished,
            this, &FirebaseDatabase::uploadMatchRequestOnFinish);
}

QJsonObject FirebaseDatabase::toJsonFields(Deck deck, MatchInfo matchInfo,
                                           bool player, QString playerRankClass)
{
    QJsonObject jsonCards;
    QMap<Card*, int> cards = player ? deck.cards() : deck.currentCards();
    for (Card* card : cards.keys()) {
        jsonCards.insert(QString::number(card->mtgaId),
                         QJsonObject{{ "integerValue", QString::number(cards[card]) }});
    }
    QString color = player ? deck.colorIdentity() : deck.colorIdentity(false);
    bool mulligan = player ? matchInfo.playerTakesMulligan
                           : matchInfo.opponentTakesMulligan;
    QString rank = player ? playerRankClass
                          : matchInfo.opponentInfo.opponentRankClass();
    QJsonObject jsonFields{
        {"fields", QJsonObject{
                { "cards", QJsonObject{
                        { "mapValue", QJsonObject{
                                {"fields", jsonCards}
                            }}
                    }},
                { "colors", QJsonObject{
                        { "stringValue", color }
                    }},
                {"mulligan", QJsonObject{
                        { "booleanValue", mulligan }}
                },
                {"rank", QJsonObject{
                        { "stringValue", rank }
                    }}
            }
        }
    };
    return jsonFields;
}

void FirebaseDatabase::uploadMatchRequestOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QJsonObject jsonRsp = Transformations::stringToJsonObject(reply->readAll());
#ifdef QT_DEBUG
    LOGD(QString(QJsonDocument(jsonRsp).toJson()));
#endif
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
    if (userSettings.userToken.isEmpty() || jsonPlayerMatchObj.size() == 0) {
        return;
    }
    if (!userSettings.isAuthValid()) {
        paramMatchID = matchID;
        recalRegisterPlayerMatch = true;
        firebaseAuth->refreshToken(userSettings.refreshToken);
        return;
    }

    QDate date = QDate::currentDate();
    QUrl url(QString("%1/users/%2/matches/%3/%4/%5").arg(ARENA_META_DB_URL)
             .arg(userSettings.userId).arg(date.year()).arg(date.month()).arg(matchID));
#ifdef QT_DEBUG
    LOGD(QString("Body: %1").arg(QString(QJsonDocument(jsonPlayerMatchObj).toJson())));
#endif

    createPatchRequest(url, QJsonDocument(jsonPlayerMatchObj), userSettings.userToken);
    jsonPlayerMatchObj = QJsonObject();
}
