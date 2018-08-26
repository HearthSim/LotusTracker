#ifndef MATCHSTATEDIFF_H
#define MATCHSTATEDIFF_H

#include "matchzone.h"
#include "matchzonetransfer.h"

class MatchStateDiff
{
private:
    int _gameStateId;
    QList<MatchZone> _zones;
    QMap<int, int> _idsChanged;
    QMap<int, MatchZoneTransfer> _idsZoneChanged;
public:
    MatchStateDiff(int gameStateId = 0, QList<MatchZone> zones = {}, QMap<int, int> idsChanged = {},
                   QMap<int, MatchZoneTransfer> idsZoneChanged = {}) : _gameStateId(gameStateId),
        _zones(zones), _idsChanged(idsChanged), _idsZoneChanged(idsZoneChanged) {}

    int gameStateId(){ return _gameStateId; }
    QList<MatchZone> zones(){ return _zones; }
    // orgObjectId, newObjectId
    QMap<int, int> idsChanged(){ return _idsChanged; }
    // objectId
    QMap<int, MatchZoneTransfer> idsZoneChanged() { return _idsZoneChanged; }

};

#endif // MATCHSTATEDIFF_H
