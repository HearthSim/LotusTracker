#ifndef RQTUPLOADPLAYERMATCH_H
#define RQTUPLOADPLAYERMATCH_H

#include "requestdata.h"
#include "../entity/deck.h"
#include "../entity/matchinfo.h"

#include <QDate>

class RqtRegisterPlayerMatch: public RequestData
{
public:
    RqtRegisterPlayerMatch()
    {
        _body = QJsonDocument();
        _path = "";
    }

    RqtRegisterPlayerMatch(MatchInfo matchInfo, Deck playerDeck) {
        QJsonObject jsonObj{
            {"deck", playerDeck.id},
            {"event", matchInfo.eventId},
            {"opponentDeckArch", matchInfo.getOpponentDeckArch()},
            {"opponentDeckColors", matchInfo.getOpponentDeckColorIdentity()},
            {"opponentName", matchInfo.opponentInfo.opponentName()},
            {"playerDeckArch", playerDeck.arch()},
            {"playerDeckColors", playerDeck.colorIdentity()},
            {"wins", matchInfo.playerMatchWins}
        };

        _body = QJsonDocument(jsonObj);
        _path = "";
    }

    void createPath(QString userId, QString matchID)
    {
        QDate date = QDate::currentDate();
        _path = QString("users/matches?userId=%1&date=%2&matchId=%3")
                .arg(userId).arg(date.toString("yyyy-MM-dd")).arg(matchID);
    }

};

#endif // RQTUPLOADPLAYERMATCH_H
