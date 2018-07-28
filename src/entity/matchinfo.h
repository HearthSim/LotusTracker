#ifndef MATCHINFO_H
#define MATCHINFO_H

#include "opponentinfo.h"

#include <QString>
#include <QPair>

class MatchInfo
{
public:
    OpponentInfo opponentInfo;
    QString eventId;
    bool playerGoFirst, playerTakesMulligan, opponentTakesMulligan, playerWins;

    void clear()
    {
        opponentInfo = OpponentInfo();
        playerGoFirst = false;
        playerTakesMulligan = false;
        opponentTakesMulligan = false;
        playerWins = false;
    }

};

#endif // MATCHINFO_H
