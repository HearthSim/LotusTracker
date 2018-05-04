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
        for (QChar manaSymbol : manaCost) {
            if (manaSymbol.isLetter()){
                if (!manaSymbols.contains(manaSymbol)){
                    manaSymbols << manaSymbol;
                }
            }
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
