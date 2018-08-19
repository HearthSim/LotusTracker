#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonDocument>
#include <QJsonObject>

class RequestData
{
protected:
    QJsonDocument _body;
    bool _hasDuplicateQuery = false;

public:
    QString _path;
    QString path() { return _path; }
    QJsonDocument body() { return _body; }
    bool hasDuplicateQuery(){ return _hasDuplicateQuery; }

};

#endif // REQUEST_H
