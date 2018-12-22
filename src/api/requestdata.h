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

    RequestData(){}
    RequestData(QString path): _path(path) {}
    RequestData(QString path, QJsonDocument body): _path(path), _body(body) {}
};

#endif // REQUEST_H
