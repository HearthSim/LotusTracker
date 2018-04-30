#ifndef CARD_H
#define CARD_H

#include <QRegularExpression>
#include <QString>

#include "../extensions.h"

class Card
{
private:
    QList<QChar> _manaColorIdentity;

    QList<QChar> calcManaColorIdentity()
    {
        QList<QChar> manaSymbols;
        QRegularExpression reManaSymbol("(?<=\\{)\\D(?=\\})");
        QRegularExpressionMatchIterator iterator = reManaSymbol.globalMatch(manaCost);
        while (iterator.hasNext()) {
            manaSymbols << iterator.next().captured(0).toLower().at(0);
        }
        if (manaSymbols.size() >= 4) {
            manaSymbols.clear();
            manaSymbols << QChar('m');
        }
        if (manaSymbols.isEmpty()) {
            manaSymbols << QChar(type == "Artifact" ? 'a' : 'c');
        }
        return manaSymbols;
    }

public:
    const int mtgaId;
    const QString setCode;
    const QString number;
    const QString name;
    const QString type;
    const QString manaCost;

    Card(int mtgaId, QString setCode, QString number, QString name, QString type, QString manaCost)
        : mtgaId(mtgaId), setCode(setCode), number(number), name(name), type(type), manaCost(manaCost){
        _manaColorIdentity = calcManaColorIdentity();
    }

    QList<QChar> manaColorIdentity()
    {
        return _manaColorIdentity;
    }

    QString manaColorIdentityAsString()
    {
        return Extensions::colorIdentityListToString(_manaColorIdentity);
    }

};

#endif // CARD_H
