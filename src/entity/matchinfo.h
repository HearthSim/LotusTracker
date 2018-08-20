#ifndef MATCHINFO_H
#define MATCHINFO_H

#include "entity/deck.h"
#include "opponentinfo.h"

#include <QMap>
#include <QPair>
#include <QString>

typedef enum {
    MatchMode_SINGLE,
    MatchMode_BEST_OF_3,
    MatchMode_UNKNOWN
} MatchMode;

class GameInfo
{
public:
    bool playerGoFirst, playerMulligan, opponentMulligan, isCompleted, playerWins;
    Deck opponentDeck;

    GameInfo(): playerGoFirst(false), playerMulligan(false), opponentMulligan(false),
        isCompleted(false), playerWins(false), opponentDeck(Deck()){

    }
};

class MatchInfo
{
public:
    QString eventId;
    OpponentInfo opponentInfo;
    MatchMode mode;
    QList<GameInfo> games;
    bool playerMatchWins;
    int playerGameWins, playerGameLoses;

    explicit MatchInfo(QString eventId = "", OpponentInfo opponentInfo = OpponentInfo()):
        eventId(eventId), opponentInfo(opponentInfo), mode(MatchMode_UNKNOWN),
        playerMatchWins(false), playerGameWins(0), playerGameLoses(0)
    {
        games.clear();
    }

    void createNewGame()
    {
        games << GameInfo();
    }

    GameInfo& currentGame()
    {
        return games.last();
    }

    QString getOpponentDeckColorIdentity()
    {
        QMap<Card*, int> opponentMatchCards;
        for (GameInfo gameInfo : games) {
            QMap<Card*, int> gameOpponentCards = gameInfo.opponentDeck.currentCards();
            for (Card* card : gameOpponentCards.keys()){
                if (opponentMatchCards.keys().contains(card)) {
                    opponentMatchCards[card] += gameOpponentCards[card];
                } else {
                    opponentMatchCards[card] = gameOpponentCards[card];
                }
            }
        }
        return Deck::calcColorIdentity(opponentMatchCards, true);
    }

    static QString MatchModeToString(MatchMode matchMode)
    {
        QMap<MatchMode, QString> matchModeNames = {
            {MatchMode_SINGLE, "Single"}, {MatchMode_BEST_OF_3, "Best of 3"},
            {MatchMode_UNKNOWN, "Unknown"} };
        return matchModeNames[matchMode];
    }

};

#endif // MATCHINFO_H
