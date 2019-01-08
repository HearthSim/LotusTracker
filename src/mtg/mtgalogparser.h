#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "mtgcards.h"
#include "../entity/deck.h"
#include "../entity/user.h"
#include "../entity/opponentinfo.h"
#include "../entity/matchinfo.h"
#include "../entity/matchplayer.h"
#include "../entity/matchstatediff.h"
#include "../entity/matchzone.h"
#include "../entity/matchzonetransfer.h"

#include <QJsonObject>
#include <QObject>
#include <QTimer>

class MtgaLogParser : public QObject
{
    Q_OBJECT

private:
    QRegularExpression reRawMsg, reMsgNumber, reMsgId, reMsgJson;
    MtgCards *mtgCards;
    QList<int> msgResponseNumbers;
    Deck jsonObject2Deck(QJsonObject jsonDeck);
    void parseOutcomingMsg(QPair<QString, QString> msg);
    void parseIncomingMsg(QPair<QString, QString> msg);
    void parsePlayerInventory(QString json);
    void parsePlayerInventoryUpdate(QString json);
    void parsePlayerCollection(QString json);
    void parsePlayerDecks(QString json);
    void parseEventPlayerCourse(QString json);
    void parseMatchCreated(QString json);
    void parseMatchInfo(QString json);
    void parsePlayerRankInfo(QString json);
    void parsePlayerRankUpdated(QString json);
    void parsePlayerDeckCreate(QString json);
    void parsePlayerDeckUpdate(QString json);
    void parsePlayerDeckSubmited(QString json);
    void parseDirectGameChallenge(QString json);
    void parseEventFinish(QString json);
    void parseSubmitDeckResp(QJsonObject jsonMessage);
    void parseClientToGreMessages(QString json);
    void parseGreToClientMessages(QString json);
    void parseGameStateFull(QJsonObject jsonMessage);
    void parseGameStateDiff(int playerSeatId, int gameStateId, QJsonObject jsonMessage);
    void parseDraftStatus(QString json);
    void checkMulligans(int playerSeatId, QList<int> diffDeletedInstanceIds,
                        QList<MatchZone> zones);
    bool listContainsSublist(QList<int> list, QList<int> subList);
    QList<MatchZone> getMatchZones(QJsonObject jsonGameStateMessage);
    QMap<int, int> getIdsChanged(QJsonArray jsonGSMAnnotations);
    QMap<int, MatchZoneTransfer> getIdsZoneChanged(QJsonArray jsonGSMAnnotations);
    QMap<int, int> getGameResults(QJsonArray jsonResults);

public:
    explicit MtgaLogParser(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    ~MtgaLogParser();
    void parse(QString logNewContent);

signals:
    void sgnPlayerInventory(PlayerInventory playerInventory);
    void sgnPlayerInventoryUpdate(QList<int> newCards);
    void sgnPlayerCollection(QMap<int, int> ownedCards);
    void sgnPlayerDecks(QList<Deck> playerDecks);
    void sgnEventPlayerCourse(QString eventId, Deck currentDeck);
    void sgnMatchCreated(QString eventId, OpponentInfo opponentInfo);
    void sgnMatchInfoSeats(QList<MatchPlayer>);
    void sgnGameStart(MatchMode mode, QList<MatchZone> zones, int seatId);
    void sgnGameCompleted(QMap<int, int> teamIdWins);
    void sgnMatchResult(int winningTeamId);
    void sgnPlayerRankInfo(QPair<QString, int> playerRankInfo);
    void sgnPlayerRankUpdated(QPair<QString, int> playerNewRank);
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
    void sgnDraftStatus(QString eventId, QString status, int packNumber, int pickNumber,
                        QList<Card*> availablePicks, QList<Card*> pickedCards);

public slots:
};

#endif // MTGALOGPARSER_H
