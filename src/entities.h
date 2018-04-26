#ifndef ENTITIES_H
#define ENTITIES_H

#include <QString>

class Card
{
public:
    const int mtgaId;
    const QString number;
    const QString name;
    const QString setCode;

	Card(const int mtgaId, const QString number, const QString name, const QString setCode) 
        : mtgaId(mtgaId), number(number), name(name), setCode(setCode){}
};

#endif // ENTITIES_H
