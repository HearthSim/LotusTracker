#ifndef UNTAPPEDAPI_H
#define UNTAPPEDAPI_H

#include <QObject>
#include <QNetworkAccessManager>

class UntappedAPI : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager networkManager;

public:
    explicit UntappedAPI(QObject *parent = nullptr);
    ~UntappedAPI();

    void fetchAnonymousUploadToken();

signals:
    void sgnNewAnonymousUploadToken(QString uploadToken);

private slots:
    void anonymousUploadTokenOnFinish();

};

#endif // UNTAPPEDAPI_H
