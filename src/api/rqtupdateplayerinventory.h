#ifndef RQTUPDATEPLAYERINVENTORY_H
#define RQTUPDATEPLAYERINVENTORY_H

#include "requestdata.h"
#include "../entity/user.h"

class RqtUpdatePlayerInventory : public RequestData
{
public:
    RqtUpdatePlayerInventory(QString userId, PlayerInventory playerInventory, QString appVersion) {
        QJsonObject jsonObj{
            {"userId", userId},
            {"wcCommon", playerInventory.getWcCommon()},
            {"wcUncommon", playerInventory.getWcUncommon()},
            {"wcRare", playerInventory.getWcRare()},
            {"wcMythic", playerInventory.getWcMythic()},
            {"appVersion", appVersion}
        };
        _body = QJsonDocument(jsonObj);
        _path = "users/extras";
    }
    ~RqtUpdatePlayerInventory() {}
};

#endif // RQTUPDATEPLAYERINVENTORY_H
