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

    bool isValid()
    {
        if (expiresTokenEpoch == 0) {
            return false;
        }
        using namespace std::chrono;
        time_point<system_clock> now = system_clock::now();
        if (now.time_since_epoch().count() > expiresTokenEpoch) {
            return false;
        }
        return true;
    }

};

#endif // USER_H
