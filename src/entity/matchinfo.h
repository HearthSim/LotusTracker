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

    explicit MatchInfo(QString eventId = "", OpponentInfo opponentInfo = OpponentInfo());
    void createNewGame();
    GameInfo& currentGame();
    QMap<Card*, int> getOpponentMatchesCards();
    QString getOpponentDeckArch();
    QString getOpponentDeckColorIdentity();

    static QString MatchModeToString(MatchMode matchMode);

};

#endif // MATCHINFO_H
