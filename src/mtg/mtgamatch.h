#ifndef MTGAMATCH_H
#define MTGAMATCH_H

#include "../entity/card.h"
#include "../entity/matchinfo.h"
#include "../entity/matchstatediff.h"
#include "../entity/matchplayer.h"
#include "../entity/matchzone.h"
#include "../entity/matchzonetransfer.h"
#include "../mtg/mtgcards.h"

#include <QObject>

typedef enum {
    TRANSFER_CAST,
    TRANSFER_COUNTERED,
    TRANSFER_DESTROY,
    TRANSFER_DISCARD,
    TRANSFER_DISCARD_FROM_LIBRARY,
    TRANSFER_DRAW,
    TRANSFER_EXILE,
    TRANSFER_PLAY,
    TRANSFER_PUT_ON_BATTLEFIELD,
    TRANSFER_PUT_ON_TOP,
    TRANSFER_RESOLVE,
    TRANSFER_RETURN,
    TRANSFER_UNKOWN
} ZoneTransferType;

class MtgaMatch : public QObject
{
    Q_OBJECT
private:
    MtgCards *mtgCards;
    MatchInfo matchInfo;
    MatchPlayer player;
    MatchPlayer opponent;
    QMap<int, MatchZone> zones;
    // objectId, ownerId
    QMap<int, int> stackOwnerTrack;
    // objectId, zoneType
    QMap<int, ZoneType> stackZoneSrcTrack;
    int currentTurn;
    bool playerGoFirst, resultPlayerWins;
    void updateZones(MatchStateDiff matchStateDiff);
    void updateIdsChanged(MatchStateDiff matchStateDiff);
    void notifyHandCardsDraw(MatchStateDiff matchStateDiff);
    void notifyCardZoneChange(int objectId, int oldObjectId, MatchZone zoneSrc,
                              MatchZone zoneDst, ZoneTransferType zoneTransferType);
    Card* getCardByObjectId(MatchZone zoneDst, int objectId);
    QString getOwnerIdentifier(int objectId, MatchZone zoneSrc);
    ZoneTransferType getZoneTransferType(int objectId, MatchZone zoneSrc,
                                         MatchZone zoneDst, ZoneTransferCategory category);

public:
    explicit MtgaMatch(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    void startNewMatch(MatchInfo matchInfo);
    void endCurrentMatch(int winningTeamId);

signals:
    void sgnPlayerPutInLibraryCard(Card* card);
    void sgnPlayerDrawCard(Card* card);
    void sgnPlayerPlayCard(Card* card);
    void sgnPlayerDiscardCard(Card* card);
    void sgnPlayerDiscardFromLibraryCard(Card* card);
    void sgnPlayerPutOnBattlefieldCard(Card* card);
    void sgnOpponentPutInLibraryCard(Card* card);
    void sgnOpponentDrawCard();
    void sgnOpponentPlayCard(Card* card);
    void sgnOpponentDiscardCard(Card* card);
    void sgnOpponentDiscardFromLibraryCard(Card* card);
    void sgnOpponentPutOnBattlefieldCard(Card* card);

public slots:
    void onMatchInfoSeats(QList<MatchPlayer> players);
    void onSeatIdThatGoFirst(int seatId);
    void onPlayerTakesMulligan();
    void onOpponentTakesMulligan(int opponentSeatId);
    void onMatchStartZones(QList<MatchZone> zones);
    void onMatchStateDiff(MatchStateDiff matchStateDiff);
    void onNewTurnStarted(int turnNumber);
};

#endif // MTGAMATCH_H
