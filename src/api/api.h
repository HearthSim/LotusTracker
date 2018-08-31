#ifndef DATABASE_H
#define DATABASE_H

#include "requestdata.h"
#include "rqtuploadplayermatch.h"
#include "entity/deck.h"
#include "entity/user.h"
#include "mtg/mtgamatch.h"

#include <QBuffer>
#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>

class LotusTrackerAPI : public QObject
{
    Q_OBJECT
private:
    bool isRefreshTokenInProgress;
    RqtRegisterPlayerMatch rqtRegisterPlayerMatch;
    QNetworkAccessManager networkManager;
    QDateTime lastUpdatePlayerCollectionDate, lastUpdatePlayerInventoryDate;
    //Params for recall after refresh token
    QMap<QString, QPair<QString, RequestData>> requestsToRecall;  //url, <method, request>
    Deck paramDeck;

    qlonglong getExpiresEpoch(QString expiresIn);
    UserSettings createUserSettingsFromRefreshedToken(QJsonObject jsonRsp);
    void getPlayerDeckToUpdate(QString deckID);
    void getPlayerDeckToUpdateRequestOnFinish();
    void getPlayerDeckWinRateRequestOnFinish();
    Deck jsonToDeck(QJsonObject deckJson);
    void registerPlayerMatch(QString matchID);
    void uploadMatchRequestOnFinish();
    QNetworkRequest prepareRequest(RequestData requestData,
                                   bool checkUserAuth, QString method = "");
    QBuffer* prepareBody(RequestData requestData);
    void sendPatch(RequestData requestData);
    void sendPost(RequestData requestData);
    void requestOnFinish();

public:
    explicit LotusTrackerAPI(QObject *parent = nullptr);
    ~LotusTrackerAPI();
    // Auth
    void refreshToken(QString refreshToken);
    //
    void updatePlayerCollection(QMap<int, int> ownedCards);
    void updatePlayerInventory(PlayerInventory playerInventory);
    void createPlayerDeck(Deck deck);
    void updatePlayerDeck(Deck deck);
    void getPlayerDeckWinRate(QString deckId, QString eventId);
    void uploadMatch(MatchInfo matchInfo, Deck playerDeck,
                     QString playerRankClass);

signals:
    void sgnRequestFinished();
    void sgnTokenRefreshed();
    void sgnTokenRefreshError();
    void sgnDeckWinRate(int wins, int losses, double winRate);

private slots:
    void tokenRefreshRequestOnFinish();
    void onTokenRefreshed();

};

#endif // DATABASE_H
