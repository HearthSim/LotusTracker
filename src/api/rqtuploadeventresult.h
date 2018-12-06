#ifndef RQTUPLOADEVENTRESULT_H
#define RQTUPLOADEVENTRESULT_H

#include "requestdata.h"
#include "../entity/deck.h"
#include "../entity/matchinfo.h"

class RqtUploadEventResult: public RequestData
{
public:
    RqtUploadEventResult()
    {
        _body = QJsonDocument();
        _path = "";
    }

    RqtUploadEventResult(QString userId, QString eventId, QString deckId, int maxWins,
                         int wins, int losses, QList<QString> matchesIds) {
        QJsonArray matches;
        for (QString matchId : matchesIds) {
            matches.append(matchId);
        }
        QJsonObject jsonObj{
            {"userId", userId},
            {"deckId", deckId},
            {"eventId", eventId},
            {"maxWins", maxWins},
            {"wins", wins},
            {"losses", losses},
            {"matchesIds", matches}
        };

        _body = QJsonDocument(jsonObj);
        _path = "users/events";
    }

};

#endif // RQTUPLOADEVENTRESULT_H
