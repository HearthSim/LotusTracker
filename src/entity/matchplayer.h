#ifndef MATCHPLAYER_H
#define MATCHPLAYER_H

#include <QString>

class MatchPlayer
{
private:
    QString _name;
    int _seatId;
    int _teamId;
public:

    MatchPlayer(QString name = "", int seatId = 0, int teamId = 0)
        : _name(name), _seatId(seatId), _teamId(teamId){}

    QString name(){ return _name; }
    int seatId(){ return _seatId; }
    int teamId(){ return _teamId; }

};

#endif // MATCHPLAYER_H
