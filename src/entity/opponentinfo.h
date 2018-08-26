#ifndef OPPONENTINFO_H
#define OPPONENTINFO_H

#include <QString>

class OpponentInfo
{
private:
    QString _opponentName;
    QString _opponentRankClass;
    int _opponentRankTier;
public:

    OpponentInfo(QString opponentName = "", QString opponentRankClass = "", int opponentRankTier = 0)
        : _opponentName(opponentName), _opponentRankClass(opponentRankClass),
          _opponentRankTier(opponentRankTier) {}

    QString opponentName(){ return _opponentName; }
    QString opponentRankClass(){ return _opponentRankClass; }
    int opponentRankTier(){ return _opponentRankTier; }

};

#endif // OPPONENTINFO_H
