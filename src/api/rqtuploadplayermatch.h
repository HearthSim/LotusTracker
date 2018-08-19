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
            {"playerDeckColors", playerDeck.colorIdentity(),
            {"opponentName", matchInfo.opponentInfo.opponentName()},
            {"opponentDeckColors", matchInfo.getOpponentDeckColorIdentity()},
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
