#ifndef MATCH_H
#define MATCH_H

class Match
{
public:
    const QString opponentName;
    const QString opponentRankClass;
    const int opponentRankTier;

    Match(QString opponentName = "", QString opponentRankClass = "", int opponentRankTier = 0)
        : opponentName(opponentName), opponentRankClass(opponentRankClass),
          opponentRankTier(opponentRankTier) {}
};

#endif // MATCH_H
