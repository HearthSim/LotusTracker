#ifndef RQTUPDATEPLAYERINVENTORY_H
#define RQTUPDATEPLAYERINVENTORY_H

#include "requestdata.h"
#include "../entity/user.h"

class RqtUpdatePlayerInventory : public RequestData
{
public:
    RqtUpdatePlayerInventory(QString userName, PlayerInventory playerInventory,
                             QString appVersion) {
        QJsonObject jsonObj{
            {"userName", userName},
            {"gold", playerInventory.getGold()},
            {"gems", playerInventory.getGems()},
            {"wcCommon", playerInventory.getWcCommon()},
            {"wcUncommon", playerInventory.getWcUncommon()},
            {"wcRare", playerInventory.getWcRare()},
            {"wcMythic", playerInventory.getWcMythic()},
            {"vaultProgress", playerInventory.getVaultProgress()},
            {"appVersion", appVersion}
        };
        _body = QJsonDocument(jsonObj);
        _path = "users/extras";
    }
    ~RqtUpdatePlayerInventory() {}
};

#endif // RQTUPDATEPLAYERINVENTORY_H
