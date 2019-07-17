#ifndef GAMEINFO_H
#define GAMEINFO_H

#include "deck.h"
#include "gamedetails.h"
#include "resultspec.h"

#include <QDateTime>
#include <QElapsedTimer>

class GameDetails
{
private:
    QElapsedTimer timer;

public:
    GameInfo gameInfo;
    bool playerGoFirst, playerMulligan, opponentMulligan, isCompleted, playerWins;
    int activePlayer, decisionPlayer, duration, turns;
    qint64 startTime;
    Deck playerDeck;
    Deck opponentRevealedDeck;
    ResultSpec resultSpec;

    GameDetails(GameInfo gameInfo, int activePlayer = 0, int decisionPlayer = 0):
        gameInfo(gameInfo), playerGoFirst(false), playerMulligan(false),
        opponentMulligan(false), isCompleted(false), playerWins(false),
        activePlayer(activePlayer), decisionPlayer(decisionPlayer), duration(0),
        turns(0), startTime(QDateTime::currentMSecsSinceEpoch()), playerDeck(Deck()),
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
