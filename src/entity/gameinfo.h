#ifndef GAMEINFO_H
#define GAMEINFO_H

#include "deck.h"
#include "gamedetails.h"
#include "resultspec.h"

#include <QElapsedTimer>

class GameInfo
{
private:
    QElapsedTimer timer;

public:
    GameDetails details;
    bool playerGoFirst, playerMulligan, opponentMulligan, isCompleted, playerWins;
    int duration;
    Deck playerDeck;
    Deck opponentRevealedDeck;
    ResultSpec resultSpec;

    GameInfo(GameDetails details): details(details), playerGoFirst(false),
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
