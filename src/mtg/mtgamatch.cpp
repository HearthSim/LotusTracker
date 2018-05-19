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
}

void MtgaMatch::onSeatIdThatGoFirst(int seatId)
{
    playerGoFirst = player.seatId() == seatId;
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
    QMap<int, QPair<int, int>> idsZoneChanged = matchStateDiff.idsZoneChanged();
    for (int objectId : idsZoneChanged.keys()) {
        MatchZone zoneSrc = zones[idsZoneChanged[objectId].first];
        MatchZone zoneDst = zones[idsZoneChanged[objectId].second];
        if (zoneDst.type() == ZoneType_STACK) {
            stackOwnerTrack[objectId] = zoneSrc.ownerSeatId();
            stackZoneSrcTrack[objectId] = zoneSrc.type();
        }
        ZoneTransferType zoneTransferType = getZoneTransferType(objectId, zoneSrc, zoneDst);
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
    QString baseMsg = QString("Turn %1 - %2").arg(currentTurn).arg(ownerIdenfitier);
    switch (zoneTransferType) {
        case TRANSFER_DRAW: {
            LOGI(QString("%1 draw %2").arg(baseMsg).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerDrawCard(card);
            } else {
                emit sgnOpponentDrawCard();
            }
            break;
        }
        case TRANSFER_PLAY: {
            LOGI(QString("%1 play %2").arg(baseMsg).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerPlayCard(card);
            } else {
                emit sgnOpponentPlayCard(card);
            }
            break;
        }
        case TRANSFER_DESTROY: {
            LOGI(QString("Turn %1 - %2 destroyed").arg(currentTurn).arg(cardName));
            break;
        }
        case TRANSFER_EXILE: {
            LOGI(QString("Turn %1 - %2 exiled").arg(currentTurn).arg(cardName));
            break;
        }
        default: {
            LOGI(QString("*** %1 move %2 from %3 to %4").arg(baseMsg)
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

ZoneTransferType MtgaMatch::getZoneTransferType(int objectId, MatchZone zoneSrc, MatchZone zoneDst)
{
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_HAND) {
        return TRANSFER_DRAW;
    }
    if (zoneSrc.type() == ZoneType_HAND && zoneDst.type() == ZoneType_BATTLEFIELD) {
        return TRANSFER_PLAY;
    }
    if (zoneSrc.type() == ZoneType_BATTLEFIELD && zoneDst.type() == ZoneType_GRAVEYARD) {
        return TRANSFER_DESTROY;
    }
    if (zoneSrc.type() == ZoneType_BATTLEFIELD && zoneDst.type() == ZoneType_EXILE) {
        return TRANSFER_EXILE;
    }
    if (zoneSrc.type() == ZoneType_STACK && zoneDst.type() == ZoneType_BATTLEFIELD
            && stackZoneSrcTrack[objectId] == ZoneType_HAND) {
        stackZoneSrcTrack.remove(objectId);
        return TRANSFER_PLAY;
    }
    if (zoneSrc.type() == ZoneType_STACK && zoneDst.type() == ZoneType_GRAVEYARD
            && stackZoneSrcTrack[objectId] == ZoneType_HAND) {
        stackZoneSrcTrack.remove(objectId);
        return TRANSFER_PLAY;
    }
    return TRANSFER_UNKOWN;
}
