#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "mtgcards.h"
#include "../entity/deck.h"
#include "../entity/eventplayercourse.h"
#include "../entity/gamedetails.h"
#include "../entity/matchdetails.h"
#include "../entity/matchplayer.h"
#include "../entity/matchstatediff.h"
#include "../entity/matchzone.h"
#include "../entity/matchzonetransfer.h"
#include "../entity/opponentinfo.h"
#include "../entity/resultspec.h"
#include "../entity/user.h"

#include <QJsonObject>
#include <QStack>
#include <QObject>
#include <QTimer>

class MtgaLogParser : public QObject
{
    Q_OBJECT

private:
    bool matchRunning;
    QRegularExpression reRawMsg, reMsgId, reMsgJson;
    MtgCards *mtgCards;
    QList<int> msgRequestIds, msgResponseIds;
    QStack<QString> lastMatchLogMsgs;
    Deck jsonObject2DeckV1(QJsonObject jsonDeck);
    Deck jsonObject2DeckV3(QJsonObject jsonDeck);
    QMap<Card*, int> v3JsonArray2List(QJsonArray cardsV3);
    void parseOutcomingMsg(QPair<QString, QString> msg);
    void parseIncomingMsg(QPair<QString, QJsonObject> msg);
    void parseMatchMsg(QPair<QString, QString> msg);

    void parsePlayerInventory(QJsonObject json);
    void parsePlayerInventoryUpdate(QJsonObject json);
    void parsePlayerCollection(QJsonObject json);
    void parsePlayerDecks(QJsonObject json);
    void parseEventPlayerCourse(QJsonObject json);
    void parseEventPlayerCourses(QJsonObject json);
    void parseMatchCreated(QJsonObject json);
    void parseMatchInfo(QString json);
    void parsePlayerRankInfo(QJsonObject json);
    void parsePlayerRankUpdated(QJsonObject json);
    void parsePlayerMythicRatingUpdated(QJsonObject json);
    void parsePlayerDeckCreate(QJsonObject json);
    void parsePlayerDeckUpdate(QJsonObject json);
    void parsePlayerDeckSubmited(QJsonObject json);
    void parseAIPracticeOrDirectGameDeck(QString json);
    void parseEventFinish(QJsonObject json);
    void parseAuthenticate(QString json);
    void parseClientToGreMessages(QString json);
    void parseGreToClientMessages(QString json);
    void parseGameStateFull(QJsonObject jsonMessage);
    void parseGameStateDiff(int playerSeatId, int gameStateId, QJsonObject jsonMessage);
    void parseDraftStatus(QJsonObject json);
    void parseDraftPick(QString json);
    void parseLogInfo(QString json);

    void checkMulligans(int playerSeatId, QList<int> diffDeletedInstanceIds,
                        QList<MatchZone> zones);
    bool listContainsSublist(QList<int> list, QList<int> subList);
    QList<MatchZone> getMatchZones(QJsonObject jsonGameStateMessage);
    QMap<int, int> getIdsChanged(QJsonArray jsonGSMAnnotations);
    QMap<int, MatchZoneTransfer> getIdsZoneChanged(QJsonArray jsonGSMAnnotations);
    QList<QPair<int, int>> getIdsRevealedCardCreated(QJsonArray jsonGSMAnnotations, QJsonArray jsonGameObjects);

public:
    explicit MtgaLogParser(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    ~MtgaLogParser();
    void parse(QString logNewContent);
    QStack<QString> getLastMatchLog();

signals:
    void sgnMtgaClientVersion(QString version);
    void sgnPlayerInventory(PlayerInventory playerInventory);
    void sgnPlayerInventoryUpdate(QList<int> newCards);
    void sgnPlayerCollection(QMap<int, int> ownedCards);
    void sgnPlayerDecks(QList<Deck> playerDecks);
    void sgnEventPlayerCourse(EventPlayerCourse eventPlayerCourse, bool isFinished);
    void sgnEventPlayerCourses(QList<QString> events);
    void sgnMatchCreated(QString matchId, QString eventId, QMap<Card*, int> playerCommanders,
                         QString opponentName, RankInfo opponentInfo, QMap<Card*, int> opponentCommanders);
    void sgnMatchInfoSeats(QList<MatchPlayer>);
    void sgnGameStart(GameInfo gameInfo, QList<MatchZone> zones, int seatId);
    void sgnGameCompleted(ResultSpec resultSpec);
    void sgnMatchResult(ResultSpec resultSpec);
    void sgnPlayerRankInfo(QPair<QString, int> playerRankInfo);
    void sgnPlayerRankUpdated(RankInfo playerCurrentRankInfo,
                              RankInfo playerOldRankInfo, int seasonOrdinal);
    void sgnPlayerMythicRatingUpdated(double oldMythicPercentile, double newMythicPercentile,
                                      int newMythicLeaderboardPlacement);
    void sgnPlayerDeckCreated(Deck deck);
    void sgnPlayerDeckUpdated(Deck deck);
    void sgnPlayerDeckSubmited(QString eventId, Deck deck);
    void sgnPlayerDeckWithSideboardSubmited(QMap<Card*, int> mainDeck,
                                            QMap<Card*, int> sideboard);
    void sgnPlayerTakesMulligan(QMap<int, int> newHandDrawed);
    void sgnOpponentTakesMulligan(int opponentSeatId);
    void sgnMatchStateDiff(MatchStateDiff matchStateDiff);
    void sgnNewTurnStarted(int turnNumber);
    void sgnEventFinish(QString eventId, QString deckId, QString deckColors,
                        int maxWins, int wins, int losses);
    void sgnDraftPick(int mtgaId, int packNumber, int pickNumber);
    void sgnDraftStatus(QString eventId, QString status, int packNumber, int pickNumber,
                        QList<Card*> availablePicks, QList<Card*> pickedCards);
    void sgnDecodeDeckPosSideboardPayload(QString type, QString payload);
    void sgnGoingToHome();
    void sgnLeavingDraft();
    void sgnSummarizedMessage();
    void sgnActivePlayer(int player);
    void sgnDecisionPlayer(int player);

public slots:
    void onParseDeckPosSideboardJson(QJsonObject jsonMessage);
};

#endif // MTGALOGPARSER_H
