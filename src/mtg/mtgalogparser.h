#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "mtgcards.h"
#include "../entity/deck.h"
#include "../entity/user.h"
#include "../entity/matchinfo.h"
#include "../entity/matchstatediff.h"
#include "../entity/matchplayer.h"
#include "../entity/matchzone.h"
#include "../entity/matchzonetransfer.h"

#include <QJsonObject>
#include <QObject>
#include <QTimer>

class MtgaLogParser : public QObject
{
    Q_OBJECT

private:
    MtgCards *mtgCards;
    QList<int> msgResponseNumbers;
    Deck jsonObject2Deck(QJsonObject jsonDeck);
    void parseMsg(QPair<QString, QString> msg);
    void parsePlayerInventory(QString json);
    void parsePlayerInventoryUpdate(QString json);
    void parsePlayerCollection(QString json);
    void parsePlayerDecks(QString json);
    void parseMatchCreated(QString json);
    void parseMatchInfo(QString json);
    void parsePlayerRankInfo(QString json);
    void parsePlayerRankUpdated(QString json);
    void parsePlayerDeckSubmited(QString json);
    void parseClientToGreMessages(QString json);
    void parseGreToClientMessages(QString json);
    void parseDieRollResult(QJsonObject jsonMessage);
    void parseGameStateFull(QJsonObject jsonMessage);
    void parseGameStateDiff(int gameStateId, QJsonObject jsonMessage);
    QList<MatchZone> getMatchZones(QJsonObject jsonGameStateMessage);
    QMap<int, int> getIdsChanged(QJsonArray jsonGSMAnnotations);
    QMap<int, MatchZoneTransfer> getIdsZoneChanged(QJsonArray jsonGSMAnnotations);

public:
    explicit MtgaLogParser(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    ~MtgaLogParser();
    void parse(QString logNewContent);

signals:
    void sgnPlayerInventory(PlayerInventory playerInventory);
    void sgnPlayerInventoryUpdate(QList<int> newCards);
    void sgnPlayerCollection(QMap<int, int> ownedCards);
    void sgnPlayerDecks(QList<Deck> playerDecks);
    void sgnMatchCreated(MatchInfo match);
    void sgnMatchInfoSeats(QList<MatchPlayer>);
    void sgnMatchInfoResultMatch(int winningTeamId);
    void sgnPlayerRankInfo(QPair<QString, int> playerRankInfo);
    void sgnPlayerRankStatus(QPair<QString, int> playerRankStatus);
    void sgnPlayerRankUpdated(QPair<QString, int> playerNewRank);
    void sgnPlayerDeckSubmited(Deck deck);
    void sgnPlayerAcceptsHand();
    void sgnPlayerCardHoverStarts();
    void sgnPlayerCardHoverEnds();
    void sgnPlayerTakesMulligan();
    void sgnSeatIdThatGoFirst(int seatId);
    void sgnMatchStartZones(QList<MatchZone> zones);
    void sgnMatchStateDiff(MatchStateDiff matchStateDiff);
    void sgnNewTurnStarted(int turnNumber);
    void sgnOpponentTakesMulligan(int opponentSeatId);

public slots:
};

#endif // MTGALOGPARSER_H
