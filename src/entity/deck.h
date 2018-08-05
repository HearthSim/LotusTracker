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
    QMap<Card*, int> cardsCurrent;
    QMap<Card*, int> cardsSideboard;

    QString calcColorIdentity(bool includeLands)
    {
        QList<QChar> distinctManaSymbols;
        for (Card *card : cardsCurrent.keys()) {
            if (card->isLand && !includeLands) {
                continue;
            }
            for (QChar symbol : card->manaColorIdentity) {
                if (distinctManaSymbols.contains(QChar('m'))) {
                    return QString("m");
                }
                if (symbol != QChar('a') && symbol != QChar('c') && !distinctManaSymbols.contains(symbol)) {
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

public:
    QString id;
    QString name;
    bool showOnlyRemainingCards;

    Deck(QString id = "", QString name = "",
         QMap<Card*, int> cards = {}, QMap<Card*, int> sideboard = {})
        : id(id), name(name), showOnlyRemainingCards(false){
        cardsCurrent = cards;
        cardsSideboard = sideboard;
        for (Card *card : cards.keys()) {
            cardsInitial[card] = cards[card];
        }
        _colorIdentity = calcColorIdentity(false);
    }

    QMap<Card*, int> cards()
    {
        return cardsInitial;
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

    void clear()
    {
        cardsCurrent.clear();
    }

    void reset()
    {
        for (Card *card : cardsInitial.keys()) {
            cardsCurrent[card] = cardsInitial[card];
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
        if (!useStartCalc) {
            return calcColorIdentity(includeLands);
        }
        return _colorIdentity;
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

};

#endif // DECK_H
