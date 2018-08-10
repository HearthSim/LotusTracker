#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "../transformations.h"

#include <QString>
#include <QMap>

class Deck
{
private:
    QString _colorIdentity;
    QMap<Card*, int> cardsInitial;
    QMap<Card*, int> cardsInitialWithSideboard;
    QMap<Card*, int> cardsCurrent;
    QMap<Card*, int> cardsSideboard;

public:
    QString id;
    QString name;
    bool showOnlyRemainingCards;

    Deck(QString id = "", QString name = "",
         QMap<Card*, int> cards = {}, QMap<Card*, int> sideboard = {})
        : id(id), name(name), showOnlyRemainingCards(false){
        cardsInitial = cards;
        cardsSideboard = sideboard;
        for (Card *card : cards.keys()) {
            cardsCurrent[card] = cards[card];
        }
        _colorIdentity = Deck::calcColorIdentity(cards, false);
    }

    QMap<Card*, int> cards(bool ignoreCardsWithSideboard = false)
    {
        if (!ignoreCardsWithSideboard && !cardsInitialWithSideboard.isEmpty()) {
            return cardsInitialWithSideboard;
        } else {
            return cardsInitial;
        }
    }

    QMap<Card*, int> sideboard()
    {
        return cardsSideboard;
    }

    QMap<Card*, int> currentCards()
    {
        if (showOnlyRemainingCards) {
            QMap<Card*, int> onlyRemainingCards;
            for (Card* card : cardsCurrent.keys()) {
                int qtd = cardsCurrent[card];
                if (qtd > 0) {
                    onlyRemainingCards[card] = qtd;
                }
            }
            return onlyRemainingCards;
        }
        return cardsCurrent;
    }

    void updateCards(QMap<Card*, int> cards)
    {
        cardsInitialWithSideboard = cards;
        reset();
    }

    void clear()
    {
        cardsCurrent.clear();
    }

    void reset()
    {
        for (Card *card : cards().keys()) {
            cardsCurrent[card] = cards()[card];
        }
    }

    bool isReseted()
    {
        for (Card *card : cardsInitial.keys()) {
            if (cardsCurrent[card] != cardsInitial[card]) {
                return false;
            }
        }
        return true;
    }

    QString colorIdentity(bool useStartCalc = true, bool includeLands = false)
    {
        if (useStartCalc) {
            return _colorIdentity;
        }
        return Deck::calcColorIdentity(cardsCurrent, includeLands);
    }

    bool drawCard(Card *card)
    {
        if (cardsCurrent[card] > 0) {
            cardsCurrent[card] -= 1;
            return true;
        } else {
            return false;
        }
    }

    void insertCard(Card *card)
    {
        if (cardsCurrent.keys().contains(card)) {
            cardsCurrent[card] += 1;
        } else {
            cardsCurrent[card] = 1;
        }
    }

    int totalCards()
    {
        int totalCards = 0;
        for (Card *card : cardsCurrent.keys()) {
            totalCards += cardsCurrent[card];
        }
        return totalCards;
    }

    int totalCardsLand()
    {
        int totalLandCards = 0;
        for (Card *card : cardsCurrent.keys()) {
            if (card->isLand) {
                totalLandCards += cardsCurrent[card];
            }
        }
        return totalLandCards;
    }

    int totalCardsOfQtd(int qtd)
    {
        int totalXCards = 0;
        for (Card *card : cardsCurrent.keys()) {
            if (cardsCurrent[card] == qtd && !card->isLand) {
                totalXCards += qtd;
            }
        }
        return totalXCards;
    }

    static QString calcColorIdentity(QMap<Card*, int> cards, bool includeLands)
    {
        QList<QChar> distinctManaSymbols;
        for (Card *card : cards.keys()) {
            if (card->isLand && !includeLands) {
                continue;
            }
            for (QChar symbol : card->borderColorIdentity) {
                if (symbol != QChar('a') && symbol != QChar('c') &&
                        symbol != QChar('m') && !distinctManaSymbols.contains(symbol)) {
                    distinctManaSymbols << symbol;
                }
            }
        }
        if (distinctManaSymbols.size() >= 4) {
            return QString("m");
        } else {
            return Transformations::colorIdentityListToString(distinctManaSymbols);
        }
    }

};

#endif // DECK_H
