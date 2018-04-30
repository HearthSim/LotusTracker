#ifndef USER_H
#define USER_H

class PlayerInventory
{
public:
    const int playerId;
    const int wcCommon;
    const int wcUncommon;
    const int wcRare;
    const int wcMythic;
    const float vaultProgress;

    PlayerInventory(int playerId, int wcCommon, int wcUncommon, int wcRare, int wcMythic, float vaultProgress)
        : playerId(playerId), wcCommon(wcCommon), wcUncommon(wcUncommon), wcRare(wcRare), 
        wcMythic(wcMythic), vaultProgress(vaultProgress){}

};

#endif // USER_H
