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

    QString calcColorIdentity()
    {
        QList<QChar> distinctManaSymbols;
        for (Card *card : cards.keys()) {
            if (card->isLand) {
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
    QString name;
    QMap<Card*, int> cards;

    Deck(QString name = "", QMap<Card*, int> cards = {}): name(name), cards(cards){
        for (Card *card : cards.keys()) {
            cardsInitial[card] = cards[card];
        }
        _colorIdentity = calcColorIdentity();
    }

    QString colorIdentity()
    {
        return _colorIdentity;
    }

    bool drawCard(Card *card)
    {
        if (cards[card] > 0) {
            cards[card] -= 1;
            return true;
        } else {
            return false;
        }
    }

    void insertCard(Card *card)
    {
        if (cards.keys().contains(card)) {
            cards[card] += 1;
        } else {
            cards[card] = 1;
        }
    }

    int totalCards()
    {
        int totalCards = 0;
        for (Card *card : cards.keys()) {
            totalCards += cards[card];
        }
        return totalCards;
    }

    int totalCardsLand()
    {
        int totalLandCards = 0;
        for (Card *card : cards.keys()) {
            if (card->isLand) {
                totalLandCards += cards[card];
            }
        }
        return totalLandCards;
    }

    int totalCardsOfQtd(int qtd)
    {
        int totalXCards = 0;
        for (Card *card : cards.keys()) {
            if (cards[card] == qtd && !card->isLand) {
                totalXCards += qtd;
            }
        }
        return totalXCards;
    }

};

#endif // DECK_H
