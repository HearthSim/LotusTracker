#ifndef MATCHZONETRANSFER_H
#define MATCHZONETRANSFER_H

typedef enum {
    ZoneTransfer_RESOLVED,
    ZoneTransfer_COUNTERED,
    ZoneTransfer_UNKOWN
} ZoneTransferCategory;

class MatchZoneTransfer
{
private:
    int _zoneSrcId, _zoneDstId;
    ZoneTransferCategory _category;

public:
    MatchZoneTransfer(int zoneSrcId = 0, int zoneDstId = 0, ZoneTransferCategory category = ZoneTransfer_UNKOWN)
        : _zoneSrcId(zoneSrcId), _zoneDstId(zoneDstId), _category(category){}

    int zoneSrcId(){ return _zoneSrcId; }

    int zoneDstId(){ return _zoneDstId; }

    ZoneTransferCategory category(){ return _category; }
};

#endif // MATCHZONETRANSFER_H
