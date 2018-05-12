#ifndef MATCHSTATEDIFF_H
#define MATCHSTATEDIFF_H

#include "matchzone.h"

class MatchStateDiff
{
private:
    QList<MatchZone> _zones;
public:
    MatchStateDiff(QList<MatchZone> zones = {}): _zones(zones) {}

    QList<MatchZone> zones(){ return _zones; }
};

#endif // MATCHSTATEDIFF_H
