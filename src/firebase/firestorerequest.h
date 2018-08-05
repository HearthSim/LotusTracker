#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonDocument>
#include <QJsonObject>

class FirestoreRequest
{
protected:
    QString _path;
    QJsonDocument _body;
    bool _hasDuplicateQuery = false;

public:
    QString path() { return _path; }
    QJsonDocument body() { return _body; }
    bool hasDuplicateQuery(){ return _hasDuplicateQuery; }

};

#endif // REQUEST_H
