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
    const QString rarity;
    const QString name;
    const QString type;
    const QString layout;
    const int cmc;
    const QString rawManaCost;
    const QList<QString> manaSymbols;
    const QList<QChar> borderColors;
    const QList<QChar> colorIdentity;
    const QString imageUrl;
    const QString draftsimRank;
    const QString lsvRank;
    const QString lsvDesc;
    const bool isLand;
    const bool isArtifact;

    Card(int mtgaId = 0, int multiverseId = 0, QString setCode = "",
         QString number = "", QString rarity = "", QString name = "",
         QString type = "", QString layout = "", int cmc = 0, QString rawManaCost = "",
         QList<QString> manaSymbols = {}, QList<QChar> borderColors = {},
         QList<QChar> colorIdentity = {}, QString imageUrl = "",
         QString draftsimRank = "", QString lvsRank = "", QString lvsDesc = "",
         bool isLand = false, bool isArtifact = false)
        : mtgaId(mtgaId), multiverseId(multiverseId), setCode(setCode),
          number(number), rarity(rarity), name(name), type(type),
          layout(layout), cmc(cmc), rawManaCost(rawManaCost),
          manaSymbols(manaSymbols), borderColors(borderColors),
          colorIdentity(colorIdentity), imageUrl(imageUrl), draftsimRank(draftsimRank),
          lsvRank(lvsRank), lsvDesc(lvsDesc), isLand(isLand), isArtifact(isArtifact){}

    bool isBasicLand()
    {
        return isLand && type.contains("Basic");
    }

    int manaCostValue()
    {
        QString magicColors = "wubrg";
        int manaValue = 0;
        for (QString mana : manaSymbols) {
            manaValue += mana[0].isDigit() ? mana.toInt() :
                                             magicColors.contains(mana) ? 1 : 0;
        }
        return manaValue;
    }

    QString borderColorIdentityAsString()
    {
        return Transformations::colorIdentityListToString(borderColors);
    }

    QString manaColorIdentityAsString()
    {
        return Transformations::colorIdentityListToString(colorIdentity);
    }

};

#endif // CARD_H
