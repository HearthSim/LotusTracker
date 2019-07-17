#ifndef GAMEINFO_H
#define GAMEINFO_H

#include "deck.h"
#include "gamedetails.h"
#include "resultspec.h"

#include <QElapsedTimer>

class GameDetails
{
private:
    QElapsedTimer timer;

public:
    GameInfo gameInfo;
    bool playerGoFirst, playerMulligan, opponentMulligan, isCompleted, playerWins;
    int duration;
    Deck playerDeck;
    Deck opponentRevealedDeck;
    ResultSpec resultSpec;

    GameDetails(GameInfo gameInfo): gameInfo(gameInfo), playerGoFirst(false),
        playerMulligan(false), opponentMulligan(false), isCompleted(false),
        playerWins(false), duration(0), playerDeck(Deck()),
        opponentRevealedDeck(Deck()), resultSpec(ResultSpec()){
        timer.start();
    }

    void finish(bool playerGameWins)
    {
        duration = static_cast<int>(timer.elapsed() / 1000);
        playerWins = playerGameWins;
        isCompleted = true;
    }

};

#endif // GAMEINFO_H
