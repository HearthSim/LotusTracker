#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "mtgcards.h"
#include "../entity/deck.h"
#include "../entity/user.h"
#include "../entity/match.h"
#include "../entity/matchstatediff.h"
#include "../entity/matchplayer.h"
#include "../entity/matchzone.h"

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
    void parsePlayerDeckSelected(QString json);
    void parseClientToGreMessages(QString json);
    void parseGreToClientMessages(QString json);
    void parseDieRollResult(QJsonObject jsonMessage);
    void parseGameStateFull(QJsonObject jsonMessage);
    void parseGameStateDiff(QJsonObject jsonMessage);
    QList<MatchZone> getMatchZones(QJsonObject jsonGameStateMessage);
    QMap<int, int> getIdsChanged(QJsonArray jsonGSMAnnotations);
    QMap<int, QPair<int, int>> getIdsZoneChanged(QJsonArray jsonGSMAnnotations);

public:
    explicit MtgaLogParser(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    ~MtgaLogParser();
    void parse(QString logNewContent);

signals:
    void sgnPlayerInventory(PlayerInventory playerInventory);
    void sgnPlayerInventoryUpdate(QList<int> newCards);
    void sgnPlayerCollection(QMap<int, int> ownedCards);
    void sgnPlayerDecks(QList<Deck> playerDecks);
    void sgnMatchCreated(Match match);
    void sgnMatchInfoSeats(QList<MatchPlayer>);
    void sgnMatchInfoResultMatch(int winningTeamId);
    void sgnPlayerRankInfo(QPair<QString, int> playerRankInfo);
    void sgnPlayerDeckSelected(Deck deck);
    void sgnPlayerAcceptsHand();
    void sgnPlayerCardHoverStarts();
    void sgnPlayerCardHoverEnds();
    void sgnPlayerTakeMulligan();
    void sgnSeatIdThatGoFirst(int seatId);
    void sgnMatchStartZones(QList<MatchZone> zones);
    void sgnMatchStateDiff(MatchStateDiff matchStateDiff);
    void sgnNewTurnStarted(int turnNumber);
    void sgnPlayerDrawCard(Card* card);
    void sgnOpponentPlayCard(Card* card);

public slots:
};

#endif // MTGALOGPARSER_H
