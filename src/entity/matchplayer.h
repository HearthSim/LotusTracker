#ifndef MATCHPLAYER_H
#define MATCHPLAYER_H

#include <QString>

class MatchPlayer
{
private:
    QString _name;
    QString _accountId;
    int _seatId;
    int _teamId;

public:

    MatchPlayer(QString name = "", QString accountId = "", int seatId = 0, int teamId = 0)
        : _name(name), _accountId(accountId), _seatId(seatId), _teamId(teamId){}

    QString name(){ return _name; }
    QString accountId(){ return _accountId; }
    int seatId(){ return _seatId; }
    int teamId(){ return _teamId; }

};

#endif // MATCHPLAYER_H
