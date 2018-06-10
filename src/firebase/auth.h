#ifndef AUTH_H
#define AUTH_H

#include <QNetworkAccessManager>
#include <QObject>

class Auth : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager networkManager;

public:
    explicit Auth(QObject *parent = nullptr);
    void registerUser(QString email, QString password);

signals:
    void sgnUserCreated();

private slots:
    void registerOnFinish();

public slots:
};

#endif // AUTH_H
