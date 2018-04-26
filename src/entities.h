#ifndef ENTITIES_H
#define ENTITIES_H

#include <QString>

class Card
{
public:
    const QString &number;
    const QString &name;

	Card(const QString &number, const QString &name) 
        : number(number), name(name){}
};

#endif // ENTITIES_H
