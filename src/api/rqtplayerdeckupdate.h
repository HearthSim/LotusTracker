#ifndef RQTPLAYERDECKUPDATE_H
#define RQTPLAYERDECKUPDATE_H

#include "requestdata.h"
#include "../entity/deck.h"

#include <QDate>

class RqtPlayerDeckUpdate : public RequestData
{
private:
    bool _isValid;

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
        _isValid = jsonDiffCards.size() > 0 || jsonDiffSideboard.size() > 0;
        _body = QJsonDocument(QJsonObject{
                                  { "userId", userId },
                                  { "deckId", deck.id },
                                  { "mainDeck", jsonDiffCards },
                                  { "sideboard", jsonDiffSideboard }
                              });
        _path = "users/decks/updates";
    }
    virtual ~RqtPlayerDeckUpdate() {}

    bool isValid() {
        return _isValid;
    }

};

#endif // RQTPLAYERDECKUPDATE_H
