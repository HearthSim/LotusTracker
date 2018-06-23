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
    qlonglong getExpiresEpoch(QString expiresIn);
    UserSettings createUserSettingsFromSign(QJsonObject jsonRsp);
    UserSettings createUserSettingsFromRefreshedToken(QJsonObject jsonRsp);

public:
    explicit Auth(QObject *parent = nullptr);
    void signInUser(QString email, QString password);
    void registerUser(QString email, QString password);
    void refreshToken(QString refreshToken);

signals:
    void sgnUserLogged(bool fromSignUp);
    void sgnTokenRefreshed();

private slots:
    void authRequestOnFinish();

public slots:
};

#endif // AUTH_H
