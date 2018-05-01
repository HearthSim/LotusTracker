#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "mtgcards.h"
#include "../entity/deck.h"
#include "../entity/user.h"
#include "../entity/match.h"

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
    explicit MtgaLogParser(QObject *parent = nullptr, MtgCards* mtgCards = nullptr);
    ~MtgaLogParser();
    void parse(QString logNewContent);

signals:
    void sgnPlayerInventory(PlayerInventory playerInventory);
    void sgnPlayerInventoryUpdate(QList<int> newCards);
    void sgnPlayerCollection(QMap<int, int> ownedCards);
    void sgnPlayerDecks(QList<Deck> playerDecks);
    void sgnPlayerDeckSelected(Deck deck);
    void sgnMatchCreated(Match match);

public slots:
};

#endif // MTGALOGPARSER_H
