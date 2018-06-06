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

void MtgaMatch::endCurrentMatch(int winningTeamId)
{
    resultPlayerWins = player.teamId() == winningTeamId;
    LOGI(QString("%1 win").arg(resultPlayerWins ? "Player" : "Opponent"))
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

void MtgaMatch::onSeatIdThatGoFirst(int seatId)
{
    playerGoFirst = player.seatId() == seatId;
    LOGI(QString("%1 go first").arg(playerGoFirst ? "Player" : "Opponent"))
}

void MtgaMatch::onPlayerTakesMulligan()
{
    QMap<int, int> handObjectIds;
    for (MatchZone zone : zones.values()) {
        if (zone.type() == ZoneType_HAND && zone.ownerSeatId() == player.seatId()) {
            handObjectIds = zone.objectIds;
            break;
        }
    }
    for (int zoneId : zones.keys()) {
        MatchZone zone = zones[zoneId];
        if (zone.type() == ZoneType_LIBRARY && zone.ownerSeatId() == player.seatId()) {
            for (int objectId : handObjectIds.keys()) {
                zone.objectIds[objectId] = 0;
                Card* card = mtgCards->findCard(handObjectIds[objectId]);
                emit sgnPlayerPutInLibraryCard(card);
            }
            zones[zoneId] = zone;
            break;
        }
    }
}

void MtgaMatch::onOpponentTakesMulligan(int opponentSeatId)
{

}

void MtgaMatch::onMatchStartZones(QList<MatchZone> matchZones)
{
    for (MatchZone zone : matchZones) {
        zones[zone.id()] = zone;
    }
}

void MtgaMatch::onMatchStateDiff(MatchStateDiff matchStateDiff)
{
    // Initial player hand draws
    for (MatchZone zone : zones) {
        if (zone.type() == ZoneType_LIBRARY && zone.ownerSeatId() == player.seatId()) {
            if (zone.objectIds.size() == 60) {
                notifyHandCardsDraw(matchStateDiff);
            }
            break;
        }
    }
    updateZones(matchStateDiff);
    updateIdsChanged(matchStateDiff);
    // Analyse objects that change zone
    QMap<int, MatchZoneTransfer> idsZoneChanged = matchStateDiff.idsZoneChanged();
    for (int objectId : idsZoneChanged.keys()) {
        MatchZone zoneSrc = zones[idsZoneChanged[objectId].zoneSrcId()];
        MatchZone zoneDst = zones[idsZoneChanged[objectId].zoneDstId()];
        int oldObjectId;
        for (auto idChanged : matchStateDiff.idsChanged().toStdMap()) {
            if (idChanged.second == objectId) {
                oldObjectId = idChanged.first;
            }
        }
        ZoneTransferCategory zoneTransferCategory = idsZoneChanged[objectId].category();
        ZoneTransferType zoneTransferType = getZoneTransferType(objectId, zoneSrc,
                                                                zoneDst, zoneTransferCategory);
        notifyCardZoneChange(objectId, oldObjectId, zoneSrc, zoneDst, zoneTransferType);
    }
}

void MtgaMatch::notifyHandCardsDraw(MatchStateDiff matchStateDiff)
{
    for (MatchZone zone : matchStateDiff.zones()) {
        if (zone.type() == ZoneType_HAND && zone.ownerSeatId() == player.seatId()) {
            for(int mtgaCardId : zone.objectIds.values()) {
                Card* card = mtgCards->findCard(mtgaCardId);
                LOGD(QString("Player draw %1").arg(card->name));
                emit sgnPlayerDrawCard(card);
            }
        }
    }
}

void MtgaMatch::notifyCardZoneChange(int objectId, int oldObjectId, MatchZone zoneSrc,
                                     MatchZone zoneDst, ZoneTransferType zoneTransferType)
{
    QString ownerIdenfitier = getOwnerIdentifier(objectId, zoneSrc);
    bool isTransferFromPlayer = ownerIdenfitier == "Player";
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
            if (isTransferFromPlayer) {
                emit sgnPlayerDiscardCard(card);
            } else {
                emit sgnOpponentDiscardCard(card);
            }
            break;
        }
        case TRANSFER_DISCARD_FROM_LIBRARY: {
            LOGD(QString("%1 discarded from library").arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerDiscardFromLibraryCard(card);
            } else {
                emit sgnOpponentDiscardFromLibraryCard(card);
            }
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
        case TRANSFER_PUT_ON_BATTLEFIELD: {
            LOGD(QString("%1 put %2 on battlefield").arg(ownerIdenfitier).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerPutOnBattlefieldCard(card);
            } else {
                emit sgnOpponentPutOnBattlefieldCard(card);
            }
            break;
        }
        case TRANSFER_PUT_ON_TOP: {
            LOGD(QString("%1 put %2 on top of deck").arg(ownerIdenfitier).arg(cardName));
            break;
        }
        case TRANSFER_RETURN: {
            Card* returnedCard = nullptr;
            for (MatchZone zone : zones) {
                if (zone.type() == ZoneType_LIMBO) {
                    returnedCard = getCardByObjectId(zone, oldObjectId);
                    break;
                }
            }
            if (returnedCard) {
                LOGD(QString("%1 returned to hand").arg(returnedCard->name));
            } else {
                LOGD(QString("%1 returned to hand").arg(cardName));
            }
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

Card* MtgaMatch::getCardByObjectId(MatchZone zone, int objectId)
{
    int cardId = zone.objectIds[objectId];
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
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_BATTLEFIELD) {
        return TRANSFER_PUT_ON_BATTLEFIELD;
    }
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_LIBRARY) {
        return TRANSFER_PUT_ON_TOP;
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
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_GRAVEYARD) {
        return TRANSFER_DISCARD_FROM_LIBRARY;
    }
    if (zoneSrc.type() == ZoneType_BATTLEFIELD && zoneDst.type() == ZoneType_HAND) {
        return TRANSFER_RETURN;
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
