#ifndef RQTCREATEPLAYERDECKUPDATE_H
#define RQTCREATEPLAYERDECKUPDATE_H

#include "firestorerequest.h"
#include "../entity/deck.h"

#include <QDate>

class RqtCreatePlayerDeckUpdate : public FirestoreRequest
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
            jsonDiffCards.insert(QString::number(card->mtgaId),
                         QJsonObject{{ "integerValue", QString("%1").arg(diffCards[card]) }});
        }
        return jsonDiffCards;
    }

public:
    RqtCreatePlayerDeckUpdate(QString userId, Deck deck, Deck oldDeck){
        QJsonObject jsonDiffCards = getDiffCardsJson(deck.cards(), oldDeck.cards());
        QJsonObject jsonDiffSideboard = getDiffCardsJson(deck.sideboard(), oldDeck.sideboard());
        _body = QJsonDocument(QJsonObject{
                                  {"fields", QJsonObject{
                                       {"mainDeck", QJsonObject{
                                            {"mapValue", QJsonObject{
                                                 {"fields", jsonDiffCards}
                                             }}
                                        }},
                                       {"sideboard", QJsonObject{
                                            {"mapValue", QJsonObject{
                                                 {"fields", jsonDiffSideboard}
                                             }}
                                        }}
                                   }}
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
