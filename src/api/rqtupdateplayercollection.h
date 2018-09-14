#ifndef RQTUPDATEPLAYERCOLLECTION_H
#define RQTUPDATEPLAYERCOLLECTION_H

#include "requestdata.h"

class RqtUpdatePlayerCollection: public RequestData
{
public:
    RqtUpdatePlayerCollection(QString userId, QMap<int, int> ownedCards) {
        QJsonObject jsonCollection;
        for (int key : ownedCards.keys()) {
            jsonCollection.insert(QString::number(key), ownedCards[key]);
        }
        _body = QJsonDocument(
                    QJsonObject({
                                    { "userId", userId },
                                    { "cards", jsonCollection }
                                }));
        _path = "users/collection";
    }
};

#endif // RQTUPDATEPLAYERCOLLECTION_H
