#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonDocument>
#include <QJsonObject>

class RequestData
{
protected:
    QString _path;
    QJsonDocument _body;

public:
    QString path() { return _path; }
    QJsonDocument body() { return _body; }

};

#endif // REQUEST_H
