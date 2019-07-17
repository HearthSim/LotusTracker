#ifndef GAMEDETAILS_H
#define GAMEDETAILS_H

#include <QString>

class GameInfo
{
public:
    QString winCondition;

    GameInfo(QString winCondition = ""): winCondition(winCondition) {}
};

#endif // GAMEDETAILS_H
