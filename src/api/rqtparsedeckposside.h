#ifndef RQTPARSEDECKPOSSIDE_H
#define RQTPARSEDECKPOSSIDE_H

#include "requestdata.h"

class RqtParseDeckPosSide: public RequestData
{
public:
    RqtParseDeckPosSide(QString type, QString payload) {
        _body = QJsonDocument(
                    QJsonObject({
                                    { "type", type },
                                    { "data", payload }
                                }));
        _path = "protobuf";
    }
};

#endif // RQTPARSEDECKPOSSIDE_H
