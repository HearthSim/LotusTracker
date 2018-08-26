#ifndef APIKEYS_H
#define APIKEYS_H

#include <QObject>

class ApiKeys : public QObject
{
    Q_OBJECT
public:
    static const QString API_BASE_URL(){ return ""; }
    static const QString AUTH_KEY(){ return ""; }
};

#endif // APIKEYS_H
