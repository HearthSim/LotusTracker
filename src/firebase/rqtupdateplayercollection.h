#ifndef RQTUPDATEPLAYERCOLLECTION_H
#define RQTUPDATEPLAYERCOLLECTION_H

#include "firestorerequest.h"

class RqtUpdatePlayerCollection: public FirestoreRequest
{
public:
    RqtUpdatePlayerCollection(QString userId, QMap<int, int> ownedCards) {
        QJsonObject jsonCollection;
        for (int key : ownedCards.keys()) {
            jsonCollection.insert(QString::number(key), QJsonObject{
                                      { "integerValue", QString::number(ownedCards[key]) }
                                  });
        }
        QJsonObject jsonObj{
            {"fields", jsonCollection}
        };
        _body = QJsonDocument(jsonObj);
        _path = QString("users/%2/inventory/cards").arg(userId);
    }
};

#endif // RQTUPDATEPLAYERCOLLECTION_H
