#ifndef UNTAPPEDAPI_H
#define UNTAPPEDAPI_H

#include "../entity/untappeduploaddata.h"

#include <QObject>
#include <QPair>
#include <QNetworkAccessManager>

class UntappedAPI : public QObject
{
    Q_OBJECT
private:
    int requestRetries;
    UntappedUploadData untappedUploadData;
    QNetworkAccessManager networkManager;
    void requestS3PutUrlWithRetry();
    void uploadMatchWithRetry();

public:
    explicit UntappedAPI(QObject *parent = nullptr);
    ~UntappedAPI();

    void fetchAnonymousUploadToken();
    void requestS3PutUrl();
    void uploadMatch(UntappedUploadData untappedUploadData);

signals:
    void sgnNewAnonymousUploadToken(QString uploadToken);
    void sgnS3PutInfo(QString putUrl, QString timestamp);

private slots:
    void anonymousUploadTokenOnFinish();

};

#endif // UNTAPPEDAPI_H
