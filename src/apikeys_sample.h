#ifndef APIKEYS_H
#define APIKEYS_H

#include <QObject>

class ApiKeys : public QObject
{
    Q_OBJECT
public:
    static const QString FIREBASE_KEY(){ return ""; }
    static const QString FIREBASE_PROJECT(){ return ""; }
};

#endif // APIKEYS_H
