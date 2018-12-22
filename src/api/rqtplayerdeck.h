#ifndef RQTPLAYERDECK_H
#define RQTPLAYERDECK_H

#include "requestdata.h"
#include "../entity/deck.h"

class RqtPlayerDeck : public RequestData
{
private:
    QJsonObject cards2Json(QMap<Card*, int> cards)
    {
        QJsonObject jsonCards;
        for (Card* card : cards.keys()) {
            jsonCards.insert(QString::number(card->mtgaId), cards[card]);
        }
        return jsonCards;
    }

public:
    RqtPlayerDeck(Deck deck) {
        QJsonObject jsonCards = cards2Json(deck.cards());
        QJsonObject jsonSideboard = cards2Json(deck.sideboard());
        QJsonObject jsonObj{
            { "deckId", deck.id },
            { "arch", deck.arch() },
            { "cards", jsonCards },
            { "sideboard", jsonSideboard },
            { "colors", deck.colorIdentity() },
            { "name", deck.name }
        };
        _body = QJsonDocument(jsonObj);
        _path = "users/decks";
    }
    virtual ~RqtPlayerDeck() {}

};

#endif // RQTPLAYERDECK_H
