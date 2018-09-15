#ifndef URLS_H
#define URLS_H

#include <QObject>

class URLs : public QObject
{
    Q_OBJECT
public:
    static const QString SITE()
    {
        bool enableLocalTests = false;
        if (enableLocalTests) {
            return "http://localhost:5000";
        } else {
            return "https://blacklotusvalley-ca867.firebaseapp.com";
        }
    }

    static const QString API()
    {
        return QString("%1/%2").arg(SITE()).arg("api");
    }
};

#endif // URLS_H
