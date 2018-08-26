#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>

class Transformations final
{
public:
    static QString colorIdentityListToString(QList<QChar> distinctManaSymbols);
    static QJsonArray stringToJsonArray(QString json);
    static QJsonObject stringToJsonObject(QString json);
    static QImage applyRoundedCorners2Image(QImage image, int cornerRadius);
    static QImage toGrayscale(QImage image);
};

#endif // EXTENSIONS_H
