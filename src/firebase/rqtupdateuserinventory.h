#ifndef RQTUPDATEUSERINVENTORY_H
#define RQTUPDATEUSERINVENTORY_H

#include "firestorerequest.h"
#include "../entity/user.h"

class RqtUpdateUserInventory : public FirestoreRequest
{
public:
    RqtUpdateUserInventory(QString userId, PlayerInventory playerInventory) {
        QJsonObject jsonInventoryFields{
            { "wcCommon", QJsonObject{
                    { "integerValue", QString::number(playerInventory.getWcCommon()) }}},
            { "wcUncommon", QJsonObject{
                    { "integerValue", QString::number(playerInventory.getWcUncommon()) }}},
            { "wcRare", QJsonObject{
                    { "integerValue", QString::number(playerInventory.getWcRare()) }}},
            { "wcMythic", QJsonObject{
                    { "integerValue", QString::number(playerInventory.getWcMythic()) }}}
        };
        QJsonObject jsonObj{
            {"fields", QJsonObject{
                    {"inventory", QJsonObject{
                            {"mapValue", QJsonObject{
                                    {"fields", jsonInventoryFields}
                                }}
                        }}
                }}
        };
        _body = QJsonDocument(jsonObj);
        _path = QString("users/%2?updateMask.fieldPaths=inventory").arg(userId);
    }
    virtual ~RqtUpdateUserInventory() {}
};

#endif // RQTUPDATEUSERINVENTORY_H
