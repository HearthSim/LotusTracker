#ifndef MATCHSTATEDIFF_H
#define MATCHSTATEDIFF_H

#include "matchzone.h"

class MatchStateDiff
{
private:
    QList<MatchZone> _zones;
    QMap<int, int> _idsChanged;
    QMap<int, QPair<int, int>> _idsZoneChanged;
public:
    MatchStateDiff(QList<MatchZone> zones = {},
                   QMap<int, int> idsChanged = {},
                   QMap<int, QPair<int, int>> idsZoneChanged = {}) :
        _zones(zones), _idsChanged(idsChanged), _idsZoneChanged(idsZoneChanged) {}

    QList<MatchZone> zones(){ return _zones; }
    QMap<int, int> idsChanged(){ return _idsChanged; }
    QMap<int, QPair<int, int>> idsZoneChanged() { return _idsZoneChanged; }
};

#endif // MATCHSTATEDIFF_H
