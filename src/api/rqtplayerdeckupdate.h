#ifndef RQTPLAYERDECKUPDATE_H
#define RQTPLAYERDECKUPDATE_H

#include "requestdata.h"
#include "../entity/deck.h"

#include <QDate>

class RqtPlayerDeckUpdate : public RequestData
{
private:
    QJsonObject getDiffCardsJson(QMap<Card*, int> deckCards,
                                 QMap<Card*, int> oldDeckCards)
    {
        QMap<Card*, int> diffCards;
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
            jsonDiffCards.insert(QString::number(card->mtgaId), diffCards[card]);
        }
        return jsonDiffCards;
    }

public:
    RqtPlayerDeckUpdate(QString userId, Deck deck, Deck oldDeck){
        QJsonObject jsonDiffCards = getDiffCardsJson(deck.cards(), oldDeck.cards());
        QJsonObject jsonDiffSideboard = getDiffCardsJson(deck.sideboard(), oldDeck.sideboard());
        _body = QJsonDocument(QJsonObject{
                                  {"mainDeck", jsonDiffCards},
                                  {"sideboard", jsonDiffSideboard}
                              });
        QString date = QDate().currentDate().toString("yyyy-MM-dd");
        QString time = QTime().currentTime().toString("HH-mm");
        QString updateName = QString("%1 %2").arg(date).arg(time);
        _path = QString("users/decks/updates?userId=%1&deckId=%2")
                .arg(userId).arg(deck.id).arg(updateName);
    }
    virtual ~RqtPlayerDeckUpdate() {}

};

#endif // RQTPLAYERDECKUPDATE_H
