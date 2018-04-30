#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QString>

class Extensions final
{
public:
    static QString colorIdentityListToString(QList<QChar> distinctManaSymbols);
    static QJsonArray stringToJsonArray(QString json);
    static QJsonObject stringToJsonObject(QString json);
};

#endif // EXTENSIONS_H
