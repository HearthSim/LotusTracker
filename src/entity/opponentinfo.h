#ifndef OPPONENTINFO_H
#define OPPONENTINFO_H

#include <QString>

class RankInfo
{
private:
    QString _class;
    int _tier;
    int _step;
    int _mythicLeaderboardPlace;
    double _mythicPercentile;

public:
    RankInfo(QString rankClass = "", int rankTier = -1, int rankStep = -1,
             int mythicLeaderboardPlace = -1, double mythicPercentile = -1)
        : _class(rankClass), _tier(rankTier), _step(rankStep),
          _mythicLeaderboardPlace(mythicLeaderboardPlace), _mythicPercentile(mythicPercentile){}

    QString rankClass(){ return _class; }
    int rankTier(){ return _tier; }
    int rankStep(){ return _step; }
    int mythicLeaderboardPlace(){ return _mythicLeaderboardPlace; }
    double mythicPercentile(){ return _mythicPercentile; }

};

#endif // OPPONENTINFO_H
