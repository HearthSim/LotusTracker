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

    static const QString GA_SENTRY_DSN() {
        return "https://a123:b456@sentry.io/c789";
    }
};

#endif // CREDENTIALS_H
