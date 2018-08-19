#ifndef DATABASE_H
#define DATABASE_H

#include "auth.h"
#include "requestdata.h"
#include "rqtregisterplayermatch.h"
#include "entity/deck.h"
#include "entity/user.h"
#include "mtg/mtgamatch.h"

#include <QBuffer>
#include <QObject>
#include <QNetworkAccessManager>

class FirebaseDatabase : public QObject
{
    Q_OBJECT
private:
    FirebaseAuth *firebaseAuth;
    RqtRegisterPlayerMatch rqtRegisterPlayerMatch;
    QNetworkAccessManager networkManager;
    //Params for recall method after refresh token
    bool recallUpdatePlayerCollection, recallUpdateUserInventory, recallGetPlayerDeck;
    bool recallCreatePlayerDeck, recallUpdatePlayerDeck, recalRegisterPlayerMatch;
    QMap<int, int> paramOwnedCards;
    PlayerInventory paramPlayerInventory;
    Deck paramDeck;
    QString paramDeckID, paramMatchID;
    QString firebaseDBUrl;

    void getPlayerDeckToUpdate(QString deckID);
    void getPlayerDeckToUpdateRequestOnFinish();
    Deck jsonToDeck(QJsonObject deckJson);
    QNetworkRequest prepareRequest(RequestData firestoreRequest);
    QBuffer* prepareBody(RequestData firestoreRequest);
    void sendPatch(RequestData requestData);
    void sendPost(RequestData requestData);
    void requestOnFinish();
    void registerPlayerMatch(QString matchID);
    void uploadMatchRequestOnFinish();

public:
    explicit FirebaseDatabase(QObject *parent = nullptr, FirebaseAuth *firebaseAuth = nullptr);
    ~FirebaseDatabase();
    void updatePlayerCollection(QMap<int, int> ownedCards);
    void updateUserInventory(PlayerInventory playerInventory);
    void createPlayerDeck(Deck deck);
    void updatePlayerDeck(Deck deck);
    void uploadMatch(MatchInfo matchInfo, Deck playerDeck,
                     QString playerRankClass);

signals:
    void sgnRequestFinished();

private slots:
    void onTokenRefreshed();

};

#endif // DATABASE_H
