#ifndef MATCHPLAYER_H
#define MATCHPLAYER_H

#include <QString>

class MatchPlayer
{
public:
    QString name;
    int seat;
    int teamId;

    MatchPlayer(QString name = "", int seat = 0, int teamId = 0)
        : name(name), seat(seat), teamId(teamId){}
};

#endif // MATCHPLAYER_H
