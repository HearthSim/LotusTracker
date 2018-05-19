#ifndef MATCHINFO_H
#define MATCHINFO_H

#include <QString>

class MatchInfo
{
private:
    QString _opponentName;
    QString _opponentRankClass;
    int _opponentRankTier;
public:

    MatchInfo(QString opponentName = "", QString opponentRankClass = "", int opponentRankTier = 0)
        : _opponentName(opponentName), _opponentRankClass(opponentRankClass),
          _opponentRankTier(opponentRankTier) {}

    QString opponentName(){ return _opponentName; }
    QString opponentRankClass(){ return _opponentRankClass; }
    int opponentRankTier(){ return _opponentRankTier; }

};

#endif // MATCHINFO_H
