#ifndef SERVER_H
#define SERVER_H

#include <QObject>

class Server : public QObject
{
    Q_OBJECT
public:
    static const QString URL()
    {
        return "";
    }
    
    static const QString API_URL()
    {
        return "";
    }
};

#endif // SERVER_H
