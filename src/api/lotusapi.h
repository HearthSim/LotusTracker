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
    typedef void (LotusTrackerAPI::*LotusTrackerAPIMethod)();

    QString userName;
    bool isRefreshTokenInProgress;
    RqtRegisterPlayerMatch rqtRegisterPlayerMatch;
    QNetworkAccessManager networkManager;
    QDateTime lastUpdatePlayerCollectionDate, lastUpdatePlayerInventoryDate;
    //Params for recall after refresh token
    QMap<QString, LotusTrackerAPIMethod> requestToRecallOnFinish;
    QMap<QString, QPair<QString, RequestData>> requestsToRecall;  //url, <method, request>
    Deck paramDeck;

    UserSettings createUserSettingsFromSign(QJsonObject jsonRsp);
    UserSettings createUserSettingsFromRefreshedToken(QJsonObject jsonRsp);
    qlonglong getExpiresEpoch(QString expiresIn);
    void getPlayerDeckToUpdate(QString deckID);
    void getPlayerDeckToUpdateRequestOnFinish();
    void getPlayerDeckWinRateRequestOnFinish();
    Deck jsonToDeck(QJsonObject deckJson);
    void registerPlayerMatch(QString matchID);
    void uploadMatchRequestOnFinish();
    QNetworkRequest prepareGetRequest(RequestData requestData, bool checkUserAuth,
                                      LotusTrackerAPIMethod onRequestFinish);
    QNetworkRequest prepareRequest(RequestData requestData,
                                   bool checkUserAuth, QString method = "");
    QBuffer* prepareBody(RequestData requestData);
    void sendGet(RequestData requestData, LotusTrackerAPIMethod onRequestFinish);
    void sendPatch(RequestData requestData);
    void sendPost(RequestData requestData);
    void requestOnFinish();

public:
    explicit LotusTrackerAPI(QObject *parent = nullptr);
    ~LotusTrackerAPI();
    // Auth
    void signInUser(QString email, QString password);
    void registerUser(QString email, QString password);
    void recoverPassword(QString email);
    void refreshToken(QString refreshToken);
    //
    void updatePlayerCollection(QMap<int, int> ownedCards);
    void setPlayerUserName(QString userName);
    void updatePlayerInventory(PlayerInventory playerInventory);
    void createPlayerDeck(Deck deck);
    void updatePlayerDeck(Deck deck);
    void publishOrUpdatePlayerDeck(QString playerName, Deck deck);
    void getPlayerDeckWinRate(QString deckId, QString eventId);
    void uploadMatch(MatchInfo matchInfo, Deck playerDeck,
                     QString playerRankClass);

signals:
    void sgnUserLogged(bool fromSignUp);
    void sgnRequestFinished();
    void sgnRequestFinishedWithSuccess();
    void sgnRequestFinishedWithError();
    void sgnPasswordRecovered();
    void sgnTokenRefreshed();
    void sgnTokenRefreshError();
    void sgnDeckWinRate(int wins, int losses, double winRate);
    void sgnEventName(QString eventName);

private slots:
    void authRequestOnFinish();
    void recoverPasswordRequestOnFinish();
    void tokenRefreshRequestOnFinish();
    void onTokenRefreshed();

};

#endif // DATABASE_H
