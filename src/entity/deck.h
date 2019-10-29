#ifndef DECK_H
#define DECK_H

#include "card.h"

#include <QString>
#include <QMap>

class Deck
{
private:
    QString _arch;
    QString _colorIdentity;
    QMap<Card*, int> cardsInitial;
    QMap<Card*, int> cardsWithSideboardInitial;
    QMap<Card*, int> cardsCurrent;
    QMap<Card*, int> cardsSideboard;
    QMap<Card*, int> cardsRevealed;

public:
    QString id;
    QString name;
    int deckTileId;
    QList<QPair<int, QString>> cardSkins;
    bool showOnlyRemainingCards;

    explicit Deck(QString id = "", QString name = "",
                  QMap<Card*, int> cards = {}, QMap<Card*, int> sideboard = {},
                  int deckTileId = 0, QList<QPair<int, QString>> cardSkins = {});
    QString arch();
    QMap<Card*, int> cards(bool withSideboardChanges = false);
    QMap<Card*, int> sideboard();
    QMap<Card*, int> currentCards();
    QMap<Card*, int> getCardsRevealed();
    int totalCards();
    int totalCardsLand();
    int totalCardsOfQtd(int qtd);

    void updateCards(QMap<Card*, int> cards, QMap<Card*, int> sideboard);
    void updateTitle(QString title);
    void clear();
    void reset(bool keepSideboardChanges = false);
    bool isReseted();

    QString colorIdentity(bool useStartCalc = true, bool includeLands = false);
    bool drawCard(Card *card);
    void revealCard(Card *card);
    void insertCard(Card *card);
    void setCardQtd(Card *card, int qtd);

    static QString calcColorIdentity(QMap<Card*, int> cards, bool includeLands);

};

#endif // DECK_H
