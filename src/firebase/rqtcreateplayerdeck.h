#ifndef RQTCREATEPLAYERDECK_H
#define RQTCREATEPLAYERDECK_H

#include "firestorerequest.h"
#include "../entity/deck.h"

class RqtCreatePlayerDeck : public FirestoreRequest
{
private:
    QJsonObject cardsToJson(QMap<Card*, int> cards)
    {
        QJsonObject jsonCards;
        for (Card* card : cards.keys()) {
            jsonCards.insert(QString::number(card->mtgaId),
                             QJsonObject{{ "integerValue", QString::number(cards[card]) }});
        }
        return jsonCards;
    }

public:
    RqtCreatePlayerDeck(QString userId, Deck deck) {
        QJsonObject jsonCards = cardsToJson(deck.cards());
        QJsonObject jsonSideboard = cardsToJson(deck.sideboard());
        QJsonObject jsonObj{
            {"fields", QJsonObject{
                    { "cards", QJsonObject{
                            { "mapValue", QJsonObject{
                                    {"fields", jsonCards}
                                }}}},
                    { "sideboard", QJsonObject{
                            { "mapValue", QJsonObject{
                                    {"fields", jsonSideboard}
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
