#ifndef MATCHZONE_H
#define MATCHZONE_H

#include <QMap>
#include <QString>

typedef enum {
    ZoneType_HAND,
    ZoneType_LIBRARY,
    ZoneType_GRAVEYARD,
    ZoneType_EXILE,
    ZoneType_LIMBO,
    ZoneType_STACK,
    ZoneType_BATTLEFIELD,
    ZoneType_UNKNOWN
} ZoneType;

class MatchZone
{
private:
    int _id, _ownerSeatId;
    ZoneType _type;
public:
    // objectId, mtgCardId
    QMap<int, int> objectIds;

    MatchZone(int id = 0, int ownerSeatId = 0, ZoneType type = ZoneType_UNKNOWN, QMap<int, int> objectIds = {}) :
        _id(id), _ownerSeatId(ownerSeatId), _type(type), objectIds(objectIds){}

    int id(){ return _id; }
    QString name(){
        QMap<ZoneType, QString> zoneTypeNames = {
            {ZoneType_HAND, "Hand"}, {ZoneType_LIBRARY, "Library"},
            {ZoneType_GRAVEYARD, "Graveyard"}, {ZoneType_EXILE, "Exile"},
            {ZoneType_LIMBO, "Limbo"}, {ZoneType_STACK, "Stack"},
            {ZoneType_BATTLEFIELD, "Battlefield"} };
        return zoneTypeNames[_type];
    }
    int ownerSeatId(){ return _ownerSeatId; }
    ZoneType type(){ return _type; }

    bool hasKnownObjectIds()
    {
        for (int objectId : objectIds) {
            if (objectId > 0) {
                return true;
            }
        }
        return false;
    }

    static ZoneType zoneTypeFromName(QString zoneTypeName)
    {
        QMap<QString, ZoneType> zoneTypeNames = {
            {"ZoneType_Hand", ZoneType_HAND}, {"ZoneType_Library", ZoneType_LIBRARY},
            {"ZoneType_Graveyard", ZoneType_GRAVEYARD}, {"ZoneType_Exile", ZoneType_EXILE},
            {"ZoneType_Limbo", ZoneType_LIMBO}, {"ZoneType_Stack", ZoneType_STACK},
            {"ZoneType_Battlefield", ZoneType_BATTLEFIELD} };
        if (zoneTypeNames.keys().contains(zoneTypeName)) {
            return zoneTypeNames[zoneTypeName];
        } else {
            return ZoneType_UNKNOWN;
        }
    }

};

#endif // MATCHZONE_H
