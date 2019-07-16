#ifndef RQTUPLOADMATCH_H
#define RQTUPLOADMATCH_H

#include "requestdata.h"
#include "../entity/deck.h"
#include "../entity/matchinfo.h"

#include <QDate>

class RqtUploadMatch: public RequestData
{
public:
    RqtUploadMatch(MatchInfo matchInfo, Deck playerDeck, QString playerRankClass) {
        QJsonObject jsonGamesMapValue = games2JsonMapValue(matchInfo.games);
        QString winner = matchInfo.playerMatchWins ? "player1" : "player2";
        int playerGameWins = 0;
        int playerGameLoses = 0;
        for(GameInfo game : matchInfo.games) {
            playerGameWins += game.playerWins ? 1 : 0;
            playerGameLoses += game.playerWins ? 0 : 1;
        }
        QString gameResult = QString("%1x%2").arg(playerGameWins).arg(playerGameLoses);
        QJsonObject player1Json{
            {"arch", playerDeck.arch()},
            {"cards", cards2JsonMapValue(playerDeck.cards(true))},
            {"colors", playerDeck.colorIdentity()},
            {"deck", playerDeck.id},
            {"name", playerDeck.name},
            {"rank", playerRankClass}
        };
        QJsonObject player2Json{
            {"arch", matchInfo.getOpponentDeckArch()},
            {"colors", matchInfo.getOpponentDeckColorIdentity()},
            {"name", matchInfo.opponent.name()},
            {"rank", matchInfo.opponentRankInfo.rankClass()}
        };
        QJsonObject jsonObj{
            {"event", matchInfo.eventId},
            {"games", jsonGamesMapValue },
            {"mode", MatchInfo::MatchModeToString(matchInfo.mode)},
            {"player1", player1Json},
            {"player2", player2Json},
            {"result", gameResult},
            {"winner", winner}
        };

        _body = QJsonDocument(jsonObj);
        _path = "matches";
    }

private:
    QJsonObject games2JsonMapValue(QList<GameInfo> games){
        QJsonObject jsonGames;
        int gameNumber = 0;
        for (GameInfo gameInfo : games) {
            gameNumber += 1;
            QString first = gameInfo.playerGoFirst ? "player1" : "player2";
            QJsonObject jsonOpponentCards = cards2JsonMapValue(gameInfo.opponentRevealedDeck.currentCards());
            QJsonObject jsonGame{
                {"duration", gameInfo.duration},
                {"first", first},
                {"opponentCards", jsonOpponentCards},
                {"opponentMulligan", gameInfo.opponentMulligan },
                {"playerMulligan", gameInfo.playerMulligan },
                {"playerWins", gameInfo.playerWins }
            };
            jsonGames.insert(QString("game%1").arg(gameNumber), jsonGame);
        }
        return jsonGames;
    }

    QJsonObject cards2JsonMapValue(QMap<Card*, int> cards)
    {
        QJsonObject jsonCards;
        for (Card* card : cards.keys()) {
            jsonCards.insert(QString::number(card->mtgaId), cards[card]);
        }
        return jsonCards;
    }

};

#endif // RQTUPLOADMATCH_H
