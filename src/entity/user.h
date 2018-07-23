#ifndef USER_H
#define USER_H

class PlayerInventory
{
private:
    int wcCommon;
    int wcUncommon;
    int wcRare;
    int wcMythic;

public:

    PlayerInventory(int wcCommon = 0, int wcUncommon = 0, int wcRare = 0, int wcMythic = 0)
        : wcCommon(wcCommon), wcUncommon(wcUncommon), wcRare(wcRare), wcMythic(wcMythic){}

    int getWcCommon(){ return wcCommon; }
    int getWcUncommon(){ return wcUncommon; }
    int getWcRare(){ return wcRare; }
    int getWcMythic(){ return wcMythic; }

};

typedef enum {
    AUTH_VALID,
    AUTH_EXPIRED,
    AUTH_INVALID
} AuthState;

#include <ctime>
#include <chrono>
#include <QString>

class UserSettings
{
private:
    QString userName;

public:
    const QString userId;
    const QString userToken;
    const QString refreshToken;
    const qlonglong expiresTokenEpoch;

    UserSettings() : userId(""), userToken(""), refreshToken(""), expiresTokenEpoch(0){
        userName = "";
    }

    UserSettings(QString userId, QString userToken, QString refreshToken, qlonglong expiresTokenEpoch)
        : userId(userId), userToken(userToken), refreshToken(refreshToken), expiresTokenEpoch(expiresTokenEpoch){}

    void setUserName(QString userName) {
        this->userName = userName;
    }

    QString getUserName() {
        return userName;
    }

    AuthState getAuthStatus()
    {
        if (expiresTokenEpoch == 0) {
            return AUTH_INVALID;
        }
        using namespace std::chrono;
        time_point<system_clock> now = system_clock::now();
        if (now.time_since_epoch().count() > expiresTokenEpoch) {
            return AUTH_EXPIRED;
        }
        return AUTH_VALID;
    }

    bool isAuthValid()
    {
        return getAuthStatus() == AUTH_VALID;
    }

};

#endif // USER_H
