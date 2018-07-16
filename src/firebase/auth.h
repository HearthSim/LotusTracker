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
    UserSettings createUserSettingsFromRefreshedToken(QJsonObject jsonRsp);

public:
    explicit FirebaseAuth(QObject *parent = nullptr);
    void signInUser(QString email, QString password);
    void registerUser(QString email, QString password);
    void refreshToken(QString refreshToken);
    void recoverPassword(QString email);

signals:
    void sgnUserLogged(bool fromSignUp);
    void sgnTokenRefreshed();
    void sgnTokenRefreshError();
    void sgnRequestFinished();
    void sgnPasswordRecovered();

private slots:
    void authRequestOnFinish();
    void tokenRefreshRequestOnFinish();
    void recoverPasswordRequestOnFinish();

public slots:
};

#endif // AUTH_H
