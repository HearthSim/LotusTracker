#ifndef APIKEYS_H
#define APIKEYS_H

#include <QObject>

class ApiKeys : public QObject
{
    Q_OBJECT
public:
    static const QString FIREBASE(){ return ""; }
};

#endif // APIKEYS_H
