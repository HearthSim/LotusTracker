#ifndef RQTCREATEPLAYERDECK_H
#define RQTCREATEPLAYERDECK_H

#include "firestorerequest.h"
#include "../entity/deck.h"

class RqtCreatePlayerDeck : public FirestoreRequest
{
public:
    RqtCreatePlayerDeck(QString userId, Deck deck) {
        QJsonObject jsonCards;
        QMap<Card*, int> cards = deck.cards();
        for (Card* card : cards.keys()) {
            jsonCards.insert(QString::number(card->mtgaId),
                             QJsonObject{{ "integerValue", QString::number(cards[card]) }});
        }
        QJsonObject jsonObj{
            {"fields", QJsonObject{
                    { "cards", QJsonObject{
                            { "mapValue", QJsonObject{
                                    {"fields", jsonCards}
                                }}}},
                    { "colors", QJsonObject{
                            { "stringValue", deck.colorIdentity() }}},
                    { "name", QJsonObject{
                            { "stringValue", deck.name }}}
                }}
        };
        _body = QJsonDocument(jsonObj);
        _path = QString("users/%2/decks/%3").arg(userId).arg(deck.id);
    }
    virtual ~RqtCreatePlayerDeck() {}

};

#endif // RQTCREATEPLAYERDECK_H
