#ifndef URLS_H
#define URLS_H

#include <QObject>

class URLs : public QObject
{
    Q_OBJECT
public:
    static const QString SITE()
    {
        return "";
    }

    static const QString API()
    {
        return "";
    }
};

#endif // URLS_H
