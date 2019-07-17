#ifndef MATCHINFO_H
#define MATCHINFO_H

#include "deck.h"
#include "gameinfo.h"
#include "matchplayer.h"
#include "opponentinfo.h"
#include "resultspec.h"

#include <QMap>
#include <QPair>
#include <QString>

class MatchInfo
{
public:
    QString matchId;
    QString eventId;
    MatchPlayer player;
    RankInfo playerCurrentRankInfo;
    RankInfo playerOldRankInfo;
    MatchPlayer opponent;
    RankInfo opponentRankInfo;
    QList<GameInfo> games;
    ResultSpec resultSpec;
    bool playerMatchWins;
    int playerGameWins, playerGameLoses, seasonOrdinal, summarizedMessage;

    explicit MatchInfo(QString matchId = "", QString eventId = "",
                       RankInfo opponentInfo = RankInfo());
    void createNewGame(GameDetails details);
    GameInfo& currentGame();
    QMap<Card*, int> getOpponentMatchesCards();
    QString getOpponentDeckArch();
    QString getOpponentDeckColorIdentity();

};

#endif // MATCHINFO_H
