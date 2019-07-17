#ifndef GAMEDETAILS_H
#define GAMEDETAILS_H

#include <QString>

class GameInfo
{
public:
    QString mulliganType;
    int number;
    QString superFormat;
    QString type;
    QString variant;
    QString winCondition;

    GameInfo(QString mulliganType = "", int number = 0, QString superFormat = "",
             QString type = "", QString variant = "", QString winCondition = ""):
        mulliganType(mulliganType), number(number), superFormat(superFormat),
        type(type), variant(variant), winCondition(winCondition) {}
};

#endif // GAMEDETAILS_H
