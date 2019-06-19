#ifndef URLS_H
#define URLS_H

#include <QObject>

class URLs : public QObject
{
    Q_OBJECT
private:
    const static bool enableLocalTests = false;

public:

    static const QString API()
    {
        if (enableLocalTests) {
            return QString("%1/%2").arg("http://localhost:5000").arg("api");
        } else {
            return QString("%1/%2").arg("https://www.mtglotusvalley.com")
                    .arg("api");
        }
    }

    static const QString SITE()
    {
        if (enableLocalTests) {
            return "http://localhost:8080";
        } else {
            return "https://www.mtglotusvalley.com";
        }
    }

};

#endif // URLS_H
