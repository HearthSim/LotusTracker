#ifndef RQTUPLOADEVENTRESULT_H
#define RQTUPLOADEVENTRESULT_H

#include "requestdata.h"
#include "../entity/deck.h"
#include "../entity/matchdetails.h"
#include "../macros.h"

class RqtUploadEventResult: public RequestData
{
public:
    RqtUploadEventResult()
    {
        _body = QJsonDocument();
        _path = "";
    }

    RqtUploadEventResult(QString eventId, QString deckId, QString deckColors,
                         int maxWins, int wins, int losses) {
        QJsonObject jsonObj{
            {"deckId", deckId},
            {"deckColors", deckColors},
            {"eventId", eventId},
            {"maxWins", maxWins},
            {"wins", wins},
            {"losses", losses}
        };
        if (APP_SETTINGS->hasDraftPick(eventId)) {
            for (int i=0; i<3; i++) {
                for (int j=0; j<15; j++) {
                    QString picks = APP_SETTINGS->getDraftPicks(eventId, i, j);
                    QString picked = APP_SETTINGS->getDraftPicked(eventId, i, j);
                    jsonObj.insert(QString("draftP%1P%2C").arg(i).arg(j), picks);
                    jsonObj.insert(QString("draftP%1P%2P").arg(i).arg(j), picked);
                }
            }
            APP_SETTINGS->clearDraftPick(eventId);
        }

        _body = QJsonDocument(jsonObj);
        _path = "users/events";
    }

};

#endif // RQTUPLOADEVENTRESULT_H
