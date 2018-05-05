#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "../extensions.h"

#include <QString>
#include <QMap>

class Deck
{
private:
    QString _colorIdentity;
    QMap<Card*, int> cardsDrawed;

    QString calcColorIdentity()
    {
        QList<QChar> distinctManaSymbols;
        for (Card *card : cards.keys()) {
            for (QChar symbol : card->manaColorIdentity()) {
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
            return Extensions::colorIdentityListToString(distinctManaSymbols);
        }
    }

public:
    QString name;
    QMap<Card*, int> cards;

    Deck(QString name = "", QMap<Card*, int> cards = {}): name(name), cards(cards){
        for (Card *card : cards.keys()) {
            cardsDrawed[card] = 0;
        }
        _colorIdentity = calcColorIdentity();
    }

    QString colorIdentity()
    {
        return _colorIdentity;
    }

    void drawCard(Card *card)
    {
        cardsDrawed[card] += 1;
    }

    void insertCard(Card *card)
    {
        if (cards.keys().contains(card)) {
            cards[card] += 1;
        } else {
            cards[card] = 1;
            cardsDrawed[card] = 0;
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
            if (card->isLand()) {
                totalLandCards += cards[card];
            }
        }
        return totalLandCards;
    }

    int totalCardsOfQtd(int qtd)
    {
        int totalXCards = 0;
        for (Card *card : cards.keys()) {
            if (!card->isLand() && cards[card] == qtd) {
                totalXCards += qtd;
            }
        }
        return totalXCards;
    }

};

#endif // DECK_H
