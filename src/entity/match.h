#ifndef MATCH_H
#define MATCH_H

#include <QString>

class Match
{
private:
    QString _opponentName;
    QString _opponentRankClass;
    int _opponentRankTier;
public:

    Match(QString opponentName = "", QString opponentRankClass = "", int opponentRankTier = 0)
        : _opponentName(opponentName), _opponentRankClass(opponentRankClass),
          _opponentRankTier(opponentRankTier) {}

    QString opponentName(){ return _opponentName; }
    QString opponentRankClass(){ return _opponentRankClass; }
    int opponentRankTier(){ return _opponentRankTier; }

};

#endif // MATCH_H
