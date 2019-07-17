#ifndef UNTAPPEDUPLOADDATA_H
#define UNTAPPEDUPLOADDATA_H

#include <QByteArray>
#include <QString>

class UntappedUploadData {
public:
    QString matchId;
    QString s3PutUrl;
    QByteArray matchData;

    explicit UntappedUploadData(QString matchId, QString s3PutUrl, QByteArray matchData):
        matchId(matchId), s3PutUrl(s3PutUrl), matchData(matchData){}

};

#endif // UNTAPPEDUPLOADDATA_H
