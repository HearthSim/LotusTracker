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

class MatchDetails
{
private:
    int nextGameActivePlayer, nextGameDecisionPlayer;

public:
    QString matchId;
    QString eventId;
    MatchPlayer player;
    RankInfo playerCurrentRankInfo;
    RankInfo playerOldRankInfo;
    MatchPlayer opponent;
    RankInfo opponentRankInfo;
    QList<GameDetails> games;
    ResultSpec resultSpec;
    bool playerMatchWins;
    int playerGameWins, playerGameLoses, seasonOrdinal, summarizedMessage;

    explicit MatchDetails(QString matchId = "", QString eventId = "",
                       RankInfo opponentInfo = RankInfo());
    void createNewGame(GameInfo gameInfo);
    GameDetails& currentGame();
    QMap<Card*, int> getOpponentMatchesCards();
    QString getOpponentDeckArch();
    QString getOpponentDeckColorIdentity();

public slots:
    void onActivePlayer(int player);
    void onDecisionPlayer(int player);

};

#endif // MATCHINFO_H
