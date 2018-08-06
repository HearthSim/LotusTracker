#ifndef MATCHINFO_H
#define MATCHINFO_H

#include "opponentinfo.h"

#include <QMap>
#include <QPair>
#include <QString>

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
    int playerGameWins, playerGameLoses;
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
        playerGameLoses = 0;
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
