#ifndef UNTAPPEDAPI_H
#define UNTAPPEDAPI_H

#include <QObject>
#include <QNetworkAccessManager>

class UntappedAPI : public QObject
{
    Q_OBJECT
private:
    int requestRetries;
    QNetworkAccessManager networkManager;
    void requestS3PutUrlWithRetry();

public:
    explicit UntappedAPI(QObject *parent = nullptr);
    ~UntappedAPI();

    void fetchAnonymousUploadToken();
    void requestS3PutUrl();

signals:
    void sgnNewAnonymousUploadToken(QString uploadToken);
    void sgnS3PutInfo(QString putUrl, QString timestamp);

private slots:
    void anonymousUploadTokenOnFinish();

};

#endif // UNTAPPEDAPI_H
