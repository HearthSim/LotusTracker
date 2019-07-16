#ifndef RESULTSPEC_H
#define RESULTSPEC_H

#include <QString>

class ResultSpec
{
public:
    QString scope;
    QString result;
    int winningTeamId;
    QString reason;

    ResultSpec(QString scope = "", QString result = "",
               int winningTeamId = -1, QString reason = ""):
        scope(scope), result(result), winningTeamId(winningTeamId), reason(reason) {}

};

#endif // RESULTSPEC_H
