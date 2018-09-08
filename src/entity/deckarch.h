#ifndef DECKARCH_H
#define DECKARCH_H

#include <QMap>
#include <QString>

class DeckArch
{
public:
    int id;
    QString name;
    QString colors;
    QMap<int, double> cards;

    explicit DeckArch(int id = 0, QString name = "",
                      QString colors = "", QMap<int, double> cards = {}):
        id(id), name(name), colors(colors), cards(cards)
    {
    }

};

#endif // DECKARCH_H
