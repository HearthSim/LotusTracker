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
        _body = QJsonDocument(jsonCollection);
        _path = QString("users/collection?userId=%1").arg(userId);
    }
};

#endif // RQTUPDATEPLAYERCOLLECTION_H
