#ifndef AUTH_H
#define AUTH_H

#include <QNetworkAccessManager>
#include <QObject>

#include "../entity/user.h"

class Auth : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager networkManager;

public:
    explicit Auth(QObject *parent = nullptr);
    void signInUser(QString email, QString password);
    void registerUser(QString email, QString password);

signals:
    void sgnUserLogged(UserSettings userSettings);

private slots:
    void signOnFinish();

public slots:
};

#endif // AUTH_H
