#ifndef RQTREGISTERPLAYERMATCH_H
#define RQTREGISTERPLAYERMATCH_H

#include "firestorerequest.h"
#include "../entity/deck.h"
#include "../entity/matchinfo.h"

#include <QDate>

class RqtRegisterPlayerMatch: public FirestoreRequest
{
public:
    RqtRegisterPlayerMatch()
    {
        _body = QJsonDocument();
        _path = "";
    }

    RqtRegisterPlayerMatch(MatchInfo matchInfo, Deck playerDeck, Deck opponentDeck) {
        QJsonObject jsonObj{
            {"fields", QJsonObject{
                {"deck", QJsonObject{
                {"stringValue", playerDeck.id}
            }},
            {"event", QJsonObject{
                {"stringValue", matchInfo.eventId}
            }},
            {"opponentName", QJsonObject{
                {"stringValue", matchInfo.opponentInfo.opponentName()}
            }},
            {"opponentDeckColors", QJsonObject{
                {"stringValue", opponentDeck.colorIdentity(false)}
            }},
            {"wins", QJsonObject{
                {"booleanValue", matchInfo.playerWins}
            }}
        }}};

        _body = QJsonDocument(jsonObj);
        _path = "";
    }

    void createPath(QString userId, QString matchID)
    {
        QDate date = QDate::currentDate();
        _path = QString("users/%2/matches/%3/%4/%5").arg(userId)
                .arg(date.year()).arg(date.month()).arg(matchID);
    }

};

#endif // RQTREGISTERPLAYERMATCH_H
