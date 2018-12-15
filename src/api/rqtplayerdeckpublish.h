#ifndef RQTPUBLISHPLAYERDECK_H
#define RQTPUBLISHPLAYERDECK_H

#include "requestdata.h"
#include "../entity/deck.h"

class RqtPublishPlayerDeck : public RequestData
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
    RqtPublishPlayerDeck(QString owner, QString ownerId, Deck deck) {
        QJsonObject jsonCards = cards2Json(deck.cards());
        QJsonObject jsonSideboard = cards2Json(deck.sideboard());
        QJsonObject jsonObj{
            { "deckId", deck.id },
            { "arch", deck.arch() },
            { "cards", jsonCards },
            { "sideboard", jsonSideboard },
            { "colors", deck.colorIdentity() },
            { "name", deck.name },
            { "owner", owner },
            { "ownerId", ownerId }
        };
        _body = QJsonDocument(jsonObj);
        _path = "users/decks/publish";
    }
    virtual ~RqtPublishPlayerDeck() {}

};

#endif // RQTPUBLISHPLAYERDECK_H
