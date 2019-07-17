#ifndef RQTUPLOADPLAYERMATCH_H
#define RQTUPLOADPLAYERMATCH_H

#include "requestdata.h"
#include "../entity/deck.h"
#include "../entity/matchdetails.h"

#include <QDate>

class RqtRegisterPlayerMatch: public RequestData
{
public:
    RqtRegisterPlayerMatch()
    {
        _body = QJsonDocument();
        _path = "";
    }

    RqtRegisterPlayerMatch(MatchDetails matchDetails, Deck playerDeck) {
        QJsonObject jsonObj{
            {"deck", playerDeck.id},
            {"event", matchDetails.eventId},
            {"opponentDeckArch", matchDetails.getOpponentDeckArch()},
            {"opponentDeckColors", matchDetails.getOpponentDeckColorIdentity()},
            {"opponentName", matchDetails.opponent.name()},
            {"playerDeckArch", playerDeck.arch()},
            {"playerDeckColors", playerDeck.colorIdentity()},
            {"wins", matchDetails.playerMatchWins}
        };

        _body = QJsonDocument(jsonObj);
        _path = "";
    }

    void createPath(QString matchId)
    {
        QJsonObject jsonObj = _body.object();
        jsonObj.insert("matchId", matchId);
        _body = QJsonDocument(jsonObj);
        _path = "users/matches";
    }

};

#endif // RQTUPLOADPLAYERMATCH_H
