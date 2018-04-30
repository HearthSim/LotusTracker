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
        _colorIdentity = calcColorIdentity();
    }

    QString colorIdentity(){
        return _colorIdentity;
    }

};

#endif // DECK_H
