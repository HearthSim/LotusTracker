#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "mtgcards.h"
#include "../entity/deck.h"
#include "../entity/user.h"
#include "../entity/match.h"
#include "../entity/matchplayer.h"

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
    void parsePlayerMulliganInfo(QString json);
    void parsePlayerMatchState(QString json);

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
    void sgnPlayerDrawCard(Card* card);
    void sgnOpponentPlayCard(Card* card);

public slots:
};

#endif // MTGALOGPARSER_H
