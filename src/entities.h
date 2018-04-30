#ifndef ENTITIES_H
#define ENTITIES_H

#include <QRegularExpression>
#include <QString>
#include <QMap>

class Card
{
private:
    QList<QChar> _manaColorIdentity;

public:
    const int mtgaId;
    const QString setCode;
    const QString number;
    const QString name;
    const QString type;
    const QString manaCost;

    Card(int mtgaId, QString setCode, QString number, QString name, QString type, QString manaCost)
        : mtgaId(mtgaId), setCode(setCode), number(number), name(name), type(type), manaCost(manaCost){
        QRegularExpression reManaSymbol("(?<=\\{)\\D(?=\\})");
        QRegularExpressionMatchIterator iterator = reManaSymbol.globalMatch(manaCost);
        while (iterator.hasNext()) {
            _manaColorIdentity << iterator.next().captured(0).toLower().at(0);
        }
        if (_manaColorIdentity.size() >= 4) {
            _manaColorIdentity.clear();
            _manaColorIdentity << QChar('m');
        }
        if (_manaColorIdentity.isEmpty()) {
            _manaColorIdentity << QChar(type == "Artifact" ? 'a' : 'c');
        }
    }

    QList<QChar> manaColorIdentity()
    {
        return _manaColorIdentity;
    }

};

class Deck
{
private:
    QString _colorIdentity;

    QString calcColorIdentity()
    {
        QList<QChar> distinctManaSymbols;
        for (Card *card : cards.keys()) {
            for (QChar symbol : card->manaColorIdentity()) {
                if (distinctManaSymbols.contains(QChar('m'))) {
                    return QString("m");
                }
                if (symbol != QChar('a') && symbol != QChar('c') && !distinctManaSymbols.contains(symbol)) {
                    distinctManaSymbols << symbol;
                }
            }
        }
        if (distinctManaSymbols.size() >= 4) {
            return QString("m");
        } else {
            QString identity = "";
            if (distinctManaSymbols.contains(QChar('b'))) { identity += "b"; }
            if (distinctManaSymbols.contains(QChar('g'))) { identity += "g"; }
            if (distinctManaSymbols.contains(QChar('r'))) { identity += "r"; }
            if (distinctManaSymbols.contains(QChar('w'))) { identity += "w"; }
            if (distinctManaSymbols.contains(QChar('u'))) { identity += "u"; }
            if (identity == "bgr") { identity = "brg"; }
            if (identity == "grw") { identity = "rgw"; }
            if (identity == "bru") { identity = "ubr"; }
            if (identity == "bwu") { identity = "wub"; }
            return (!identity.isEmpty()) ? identity : "default";
        }
    }

public:
    const QString name;
    const QMap<Card*, int> cards;
    QMap<Card*, int> cardsDrawed;

    Deck(QString name = "", QMap<Card*, int> cards = {}): name(name), cards(cards){
        _colorIdentity = calcColorIdentity();
    }

    QString colorIdentity(){
        return _colorIdentity;
    }

};

#endif // ENTITIES_H
