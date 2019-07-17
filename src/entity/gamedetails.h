#ifndef GAMEDETAILS_H
#define GAMEDETAILS_H

#include <QString>

class GameDetails
{
public:
    QString winCondition;

    GameDetails(QString winCondition = ""): winCondition(winCondition) {}
};

#endif // GAMEDETAILS_H
