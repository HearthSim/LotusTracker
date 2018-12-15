#ifndef RQTUPDATEPLAYERCOLLECTION_H
#define RQTUPDATEPLAYERCOLLECTION_H

#include "requestdata.h"

class RqtUpdatePlayerCollection: public RequestData
{
public:
    RqtUpdatePlayerCollection(QMap<int, int> ownedCards) {
        QJsonObject jsonCollection;
        for (int key : ownedCards.keys()) {
            jsonCollection.insert(QString::number(key), ownedCards[key]);
        }
        _body = QJsonDocument(
                    QJsonObject({
                                    { "cards", jsonCollection }
                                }));
        _path = "users/collection";
    }
};

#endif // RQTUPDATEPLAYERCOLLECTION_H
