#ifndef AUTH_H
#define AUTH_H

#include <QNetworkAccessManager>
#include <QObject>

#include "../entity/user.h"

class FirebaseAuth : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager networkManager;
    qlonglong getExpiresEpoch(QString expiresIn);
    UserSettings createUserSettingsFromSign(QJsonObject jsonRsp);

public:
    explicit FirebaseAuth(QObject *parent = nullptr);
    void signInUser(QString email, QString password);
    void registerUser(QString email, QString password);
    void recoverPassword(QString email);

signals:
    void sgnUserLogged(bool fromSignUp);
    void sgnRequestFinished();
    void sgnPasswordRecovered();

private slots:
    void authRequestOnFinish();
    void recoverPasswordRequestOnFinish();

public slots:
};

#endif // AUTH_H
