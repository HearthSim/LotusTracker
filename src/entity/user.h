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

#endif // USER_H
