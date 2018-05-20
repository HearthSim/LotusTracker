#include "mtgamatch.h"
#include "../macros.h"

MtgaMatch::MtgaMatch(QObject *parent, MtgCards *mtgCards)
    : QObject(parent), mtgCards(mtgCards)
{

}

void MtgaMatch::startNewMatch(MatchInfo matchInfo)
{
    this->matchInfo = matchInfo;
    player = MatchPlayer();
    opponent = MatchPlayer();
    zones.clear();
    stackZoneSrcTrack.clear();
    currentTurn = 1;
    playerGoFirst = false;
    resultPlayerWins = false;
    LOGI("New match started")
}

void MtgaMatch::onMatchInfoSeats(QList<MatchPlayer> players)
{
    for (MatchPlayer matchPlayer : players) {
        if (matchPlayer.name() == matchInfo.opponentName()) {
            opponent = MatchPlayer(matchPlayer.name(), matchPlayer.seatId(), matchPlayer.teamId());
        } else {
            player = MatchPlayer(matchPlayer.name(), matchPlayer.seatId(), matchPlayer.teamId());
        }
    }
}

void MtgaMatch::onMatchInfoResultMatch(int winningTeamId)
{
    resultPlayerWins = player.teamId() == winningTeamId;
    LOGI(QString("%1 win").arg(resultPlayerWins ? "Player" : "Opponent"))
}

void MtgaMatch::onSeatIdThatGoFirst(int seatId)
{
    playerGoFirst = player.seatId() == seatId;
    LOGI(QString("%1 go first").arg(playerGoFirst ? "Player" : "Opponent"))
}

void MtgaMatch::onMatchStartZones(QList<MatchZone> matchZones)
{
    for (MatchZone zone : matchZones) {
        zones[zone.id()] = zone;
    }
}

void MtgaMatch::onMatchStateDiff(MatchStateDiff matchStateDiff)
{
    updateZones(matchStateDiff);
    updateIdsChanged(matchStateDiff);
    // Analyse objects that change zone
    QMap<int, MatchZoneTransfer> idsZoneChanged = matchStateDiff.idsZoneChanged();
    for (int objectId : idsZoneChanged.keys()) {
        MatchZone zoneSrc = zones[idsZoneChanged[objectId].zoneSrcId()];
        MatchZone zoneDst = zones[idsZoneChanged[objectId].zoneDstId()];
        ZoneTransferCategory zoneTransferCategory = idsZoneChanged[objectId].category();
        ZoneTransferType zoneTransferType = getZoneTransferType(objectId, zoneSrc,
                                                                zoneDst, zoneTransferCategory);
        notifyCardZoneChange(objectId, zoneSrc, zoneDst, zoneTransferType);
    }
}

void MtgaMatch::notifyCardZoneChange(int objectId, MatchZone zoneSrc, MatchZone zoneDst,
                                     ZoneTransferType zoneTransferType)
{
    QString ownerIdenfitier = getOwnerIdentifier(objectId, zoneSrc);
    bool isTransferFromPlayer = ownerIdenfitier == "player";
    Card* card = getCardByObjectId(zoneDst, objectId);
    QString cardName = card ? card->name : QString("Object %1").arg(objectId);
    switch (zoneTransferType) {
        case TRANSFER_DRAW: {
            LOGD(QString("%1 draw %2").arg(ownerIdenfitier).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerDrawCard(card);
            } else {
                emit sgnOpponentDrawCard();
            }
            break;
        }
        case TRANSFER_CAST:
        case TRANSFER_PLAY: {
            QString action = zoneTransferType == TRANSFER_CAST ? "cast" : "play";
            LOGD(QString("%1 %2 %3").arg(ownerIdenfitier).arg(action).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerPlayCard(card);
            } else {
                emit sgnOpponentPlayCard(card);
            }
            break;
        }
        case TRANSFER_DESTROY: {
            LOGD(QString("%1 destroyed").arg(cardName));
            break;
        }
        case TRANSFER_DISCARD: {
            LOGD(QString("%1 discarded").arg(cardName));
            break;
        }
        case TRANSFER_EXILE: {
            LOGD(QString("%1 exiled").arg(cardName));
            break;
        }
        case TRANSFER_COUNTERED: {
            LOGD(QString("%1 countered").arg(cardName));
            break;
        }
        case TRANSFER_RESOLVE: {
            LOGD(QString("%1 resolved").arg(cardName));
            break;
        }
        default: {
            LOGD(QString("*** %1 move %2 from %3 to %4").arg(ownerIdenfitier)
                 .arg(cardName).arg(zoneSrc.name()).arg(zoneDst.name()));
        }
    }
}

void MtgaMatch::onNewTurnStarted(int turnNumber)
{
    currentTurn = turnNumber;
}

void MtgaMatch::updateZones(MatchStateDiff matchStateDiff)
{
    QMap<int, MatchZone> actualZones = zones;
    for (MatchZone zone : matchStateDiff.zones()) {
        zones[zone.id()] = zone;
        // Update zone with objects from old zone
        for (int objectId : zone.objectIds.keys()) {
            if (zone.objectIds[objectId] == 0) {
                zone.objectIds[objectId] = actualZones[zone.id()].objectIds[objectId];
            }
        }
    }
}

void MtgaMatch::updateIdsChanged(MatchStateDiff matchStateDiff)
{
    for (int idChanged : matchStateDiff.idsChanged().keys()) {
        int newObjectId = matchStateDiff.idsChanged()[idChanged];
        for (MatchZone zone : zones) {
            if (zone.objectIds.keys().contains(idChanged)) {
                zone.objectIds[newObjectId] = zone.objectIds[idChanged];
                zone.objectIds.remove(idChanged);
            }
        }
        // Update temp stack ownerSeatId and ZoneType source
        stackOwnerTrack[newObjectId] = stackOwnerTrack[idChanged];
        stackOwnerTrack.remove(idChanged);
        stackZoneSrcTrack[newObjectId] = stackZoneSrcTrack[idChanged];
        stackZoneSrcTrack.remove(idChanged);
    }
}

Card* MtgaMatch::getCardByObjectId(MatchZone zoneDst, int objectId)
{
    int cardId = zoneDst.objectIds[objectId];
    if (cardId > 0) {
        return mtgCards->findCard(cardId);
    }
    return nullptr;
}

QString MtgaMatch::getOwnerIdentifier(int objectId, MatchZone zoneSrc)
{
    int ownerId = zoneSrc.type() == ZoneType_STACK ? stackOwnerTrack[objectId]
              : zoneSrc.ownerSeatId();
    if (ownerId == player.seatId()) {
        return "Player";
    }
    if (ownerId == opponent.seatId()) {
        return "Opponent";
    }
    return "Unknown";
}

ZoneTransferType MtgaMatch::getZoneTransferType(int objectId, MatchZone zoneSrc,
                                                MatchZone zoneDst, ZoneTransferCategory category)
{
    if (zoneSrc.type() == ZoneType_HAND && zoneDst.type() == ZoneType_STACK) {
        stackOwnerTrack[objectId] = zoneSrc.ownerSeatId();
        stackZoneSrcTrack[objectId] = zoneSrc.type();
        return TRANSFER_CAST;
    }
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_HAND) {
        return TRANSFER_DRAW;
    }
    if (zoneSrc.type() == ZoneType_HAND && zoneDst.type() == ZoneType_BATTLEFIELD) {
        return TRANSFER_PLAY;
    }
    if (zoneSrc.type() == ZoneType_BATTLEFIELD && zoneDst.type() == ZoneType_GRAVEYARD) {
        return TRANSFER_DESTROY;
    }
    if (zoneDst.type() == ZoneType_EXILE) {
        return TRANSFER_EXILE;
    }
    if (zoneSrc.type() == ZoneType_HAND && zoneDst.type() == ZoneType_GRAVEYARD) {
        return TRANSFER_DISCARD;
    }
    if (zoneSrc.type() == ZoneType_STACK && zoneDst.type() == ZoneType_BATTLEFIELD
            && stackZoneSrcTrack[objectId] == ZoneType_HAND) {
        stackZoneSrcTrack.remove(objectId);
        return TRANSFER_RESOLVE;
    }
    if (zoneSrc.type() == ZoneType_STACK && zoneDst.type() == ZoneType_GRAVEYARD
            && stackZoneSrcTrack[objectId] == ZoneType_HAND) {
        stackZoneSrcTrack.remove(objectId);
        if (category == ZoneTransfer_RESOLVED) {
            return TRANSFER_RESOLVE;
        }
        if (category == ZoneTransfer_COUNTERED) {
            return TRANSFER_COUNTERED;
        }
    }
    return TRANSFER_UNKOWN;
}
