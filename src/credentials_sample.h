#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <QObject>

class CREDENTIALS : public QObject
{
    Q_OBJECT

public:
    static const QString GA_ID() {
        return "UA-XXXXXXXX-X";
    }

};

#endif // CREDENTIALS_H
