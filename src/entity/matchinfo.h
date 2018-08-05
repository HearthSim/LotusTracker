#ifndef MATCHINFO_H
#define MATCHINFO_H

#include "opponentinfo.h"

#include <QString>
#include <QPair>

typedef enum {
    MatchMode_SINGLE,
    MatchMode_BEST_OF_3,
    MatchMode_UNKNOWN
} MatchMode;

class MatchInfo
{
public:
    OpponentInfo opponentInfo;
    QString eventId;
    MatchMode mode;
    int playerGameWins, totalGames;
    bool playerGoFirst, playerTakesMulligan, opponentTakesMulligan, playerWins;

    void clear()
    {
        eventId = "";
        mode = MatchMode_UNKNOWN;
        opponentInfo = OpponentInfo();
        playerGoFirst = false;
        playerTakesMulligan = false;
        opponentTakesMulligan = false;
        playerWins = false;
        playerGameWins = 0;
        totalGames = 0;
    }

};

#endif // MATCHINFO_H
