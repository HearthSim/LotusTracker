#ifndef USER_H
#define USER_H

class PlayerInventory
{
public:
    const int wcCommon;
    const int wcUncommon;
    const int wcRare;
    const int wcMythic;
    const float vaultProgress;

    PlayerInventory() : wcCommon(0), wcUncommon(0), wcRare(0), wcMythic(0), vaultProgress(0.0){}

    PlayerInventory(int wcCommon, int wcUncommon, int wcRare, int wcMythic, float vaultProgress)
        : wcCommon(wcCommon), wcUncommon(wcUncommon), wcRare(wcRare), wcMythic(wcMythic),
          vaultProgress(vaultProgress){}

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
public:
    const QString userId;
    const QString userToken;
    const QString refreshToken;
    const qlonglong expiresTokenEpoch;

    UserSettings() : userId(""), userToken(""), refreshToken(""), expiresTokenEpoch(0){}

    UserSettings(QString userId, QString userToken, QString refreshToken, qlonglong expiresTokenEpoch)
        : userId(userId), userToken(userToken), refreshToken(refreshToken), expiresTokenEpoch(expiresTokenEpoch){}

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
