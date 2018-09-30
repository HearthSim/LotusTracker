#ifndef CARD_H
#define CARD_H

#include <QRegularExpression>
#include <QString>

#include "../transformations.h"

class Card
{
public:
    const int mtgaId;
    const int multiverseId;
    const QString setCode;
    const QString number;
    const QString name;
    const QString type;
    const QString layout;
    const QString manaCost;
    const QList<QChar> borderColorIdentity;
    const QList<QChar> manaColorIdentity;
    const QString imageUrl;
    bool isLand;
    bool isArtifact;

    Card(int mtgaId = 0, int multiverseId = 0, QString setCode = "",
         QString number = "", QString name = "", QString type = "",
         QString layout = "", QString manaCost = "",
         QList<QChar> borderColorIdentity = {},
         QList<QChar> manaColorIdentity = {},
         QString imageUrl = "", bool isLand = false, bool isArtifact = false)
        : mtgaId(mtgaId), multiverseId(multiverseId), setCode(setCode),
          number(number), name(name), type(type), layout(layout), manaCost(manaCost),
          borderColorIdentity(borderColorIdentity), manaColorIdentity(manaColorIdentity),
          imageUrl(imageUrl), isLand(isLand), isArtifact(isArtifact){}

    bool isBasicLand()
    {
        return isLand && type.contains("Basic");
    }

    int manaCostValue()
    {
        int manaValue = 0;
        for (QChar mana : manaCost) {
            manaValue += mana.isDigit() ? QString(mana).toInt() : 1;
        }
        return manaValue;
    }

    QString borderColorIdentityAsString()
    {
        return Transformations::colorIdentityListToString(borderColorIdentity);
    }

    QString manaColorIdentityAsString()
    {
        return Transformations::colorIdentityListToString(manaColorIdentity);
    }

};

#endif // CARD_H
