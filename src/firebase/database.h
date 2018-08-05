#ifndef DATABASE_H
#define DATABASE_H

#include "auth.h"
#include "firestorerequest.h"
#include "rqtregisterplayermatch.h"
#include "entity/deck.h"
#include "entity/user.h"
#include "mtg/mtgamatch.h"

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

    void getPlayerDeckToUpdate(QString deckID);
    void registerPlayerMatch(QString matchID);
    void sendPatchRequest(FirestoreRequest firestoreRequest, QString userToken);
    void requestOnFinish();
    void getPlayerDeckToUpdateRequestOnFinish();
    void uploadMatchRequestOnFinish();

public:
    explicit FirebaseDatabase(QObject *parent = nullptr, FirebaseAuth *firebaseAuth = nullptr);
    ~FirebaseDatabase();
    void updatePlayerCollection(QMap<int, int> ownedCards);
    void updateUserInventory(PlayerInventory playerInventory);
    void createPlayerDeck(Deck deck);
    void updatePlayerDeck(Deck deck);
    void uploadMatch(MatchInfo matchInfo, QString playerRankClass,
                     Deck playerDeck, Deck opponentDeck);

signals:
    void sgnRequestFinished();

private slots:
    void onTokenRefreshed();

};

#endif // DATABASE_H
