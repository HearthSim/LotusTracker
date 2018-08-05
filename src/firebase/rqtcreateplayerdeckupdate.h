#ifndef RQTCREATEPLAYERDECKUPDATE_H
#define RQTCREATEPLAYERDECKUPDATE_H

#include "firestorerequest.h"
#include "../entity/deck.h"

#include <QDate>

class RqtCreatePlayerDeckUpdate : public FirestoreRequest
{
public:
    RqtCreatePlayerDeckUpdate(QString userId, Deck deck, Deck oldDeck){
        QMap<Card*, int> diffCards;
        QMap<Card*, int> deckCards = deck.cards();
        QMap<Card*, int> oldDeckCards = oldDeck.cards();
        for (Card* deckCard : deckCards.keys()) {
            if (oldDeckCards.contains(deckCard)) {
                // Add cards with diferent qtd to diffCards
                if (oldDeckCards[deckCard] != deckCards[deckCard]) {
                    diffCards[deckCard] = deckCards[deckCard] - oldDeckCards[deckCard];
                }
            } else {
                // Add new cards to diffCards
                diffCards[deckCard] = deckCards[deckCard];
            }
        }
        for (Card* oldDeckCard : oldDeckCards.keys()) {
            // Add removed cards to diffCards
            if (!deckCards.contains(oldDeckCard)) {
                diffCards[oldDeckCard] = oldDeckCards[oldDeckCard] * -1;
            }
        }

        QJsonObject jsonDiffCards;
        for (Card* card : diffCards.keys()) {
            jsonDiffCards.insert(QString::number(card->mtgaId),
                         QJsonObject{{ "integerValue", QString("%1").arg(diffCards[card]) }});
        }
        _body = QJsonDocument(QJsonObject{
                                  {"fields", jsonDiffCards}
                              });
        QString date = QDate().currentDate().toString("yyyy-MM-dd");
        QString time = QTime().currentTime().toString("HH-mm");
        QString updateName = QString("%1 %2").arg(date).arg(time);
        _path = QString("users/%2/decks/%3/updates/%4")
                .arg(userId).arg(deck.id).arg(updateName);
    }
    virtual ~RqtCreatePlayerDeckUpdate() {}

};

#endif // RQTCREATEPLAYERDECKUPDATE_H
