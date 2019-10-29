#include "mtgamatch.h"
#include "../entity/matchplayer.h"
#include "../macros.h"

MtgaMatch::MtgaMatch(QObject *parent, MtgCards *mtgCards)
    : QObject(parent), mtgCards(mtgCards), isRunning(false)
{
    playerRankInfo = qMakePair(QString(""), 0);
}

QString MtgaMatch::getPlayerName()
{
    return matchDetails.player.name();
}

MatchDetails MtgaMatch::getMatchDetails()
{
    return matchDetails;
}

QPair<QString, int> MtgaMatch::getPlayerRankInfo()
{
    return playerRankInfo;
}

void MtgaMatch::onStartNewMatch(QString matchId, QString eventId, QMap<Card*, int> playerCommanders,
                                QString opponentName, RankInfo opponentInfo, QMap<Card*, int> opponentCommanders)
{
    if (isRunning) {
        onEndCurrentMatch(ResultSpec());
    }
    this->opponentName = opponentName;
    matchDetails = MatchDetails(matchId, eventId, playerCommanders, opponentInfo, opponentCommanders);
    //don't clear playerRankInfo because it is set before startNewMatch
    gameZones.clear();
    stackOwnerTrack.clear();
    stackZoneSrcTrack.clear();
    currentTurn = 1;
    summarizedMessage = 0;
    revealedCards = {};
    revealedCardsPlayed = {};
    isRunning = true;
    LOGI("New match started")
}

void MtgaMatch::onMatchInfoSeats(QList<MatchPlayer> players)
{
    if (!isRunning) {
        return;
    }
    for (MatchPlayer matchPlayer : players) {
        if (matchPlayer.name() == opponentName) {
            matchDetails.opponent = matchPlayer;
        } else {
            matchDetails.player = matchPlayer;
        }
    }
    emit sgnPlayerUserName(matchDetails.player.name());
}

void MtgaMatch::onGameStart(GameInfo gameInfo, QList<MatchZone> zones, int seatId)
{
    if (!isRunning) {
        return;
    }
    matchDetails.createNewGame(gameInfo);
    for (MatchZone zone : zones) {
        gameZones[zone.id()] = zone;
    }
    bool playerGoFirst = matchDetails.player.seatId() == seatId;
    matchDetails.currentGame().playerGoFirst = playerGoFirst;
    LOGI(QString("%1 go first").arg(playerGoFirst ? "Player" : "Opponent"))
}

void MtgaMatch::onGameCompleted(Deck playerDeck, Deck opponentRevealedDeck,
                                ResultSpec resultSpec)
{
    matchDetails.currentGame().playerDeck = playerDeck;
    matchDetails.currentGame().opponentRevealedDeck = opponentRevealedDeck;
    int playerCurrentWins = 0;
    for(int i=0; i<matchDetails.games.size()-1; i++) {
        GameDetails game = matchDetails.games[i];
        if (game.playerWins) {
            playerCurrentWins += 1;
        }
    }
    matchDetails.currentGame().turns = currentTurn;
    matchDetails.currentGame().resultSpec = resultSpec;
    bool playerGameWins = matchDetails.player.teamId() == resultSpec.winningTeamId;
    matchDetails.currentGame().finish(playerGameWins);
}

void MtgaMatch::onEndCurrentMatch(ResultSpec resultSpec)
{
    if (!isRunning) {
        return;
    }
    isRunning = false;
    matchDetails.resultSpec = resultSpec;
    matchDetails.playerMatchWins = matchDetails.player.teamId() == resultSpec.winningTeamId;
    matchDetails.summarizedMessage = summarizedMessage;
    LOGI(QString("%1 wins").arg(matchDetails.playerMatchWins ? "Player" : "Opponent"))
}

void MtgaMatch::onPlayerRankInfo(QPair<QString, int> playerRankInfo)
{
    this->playerRankInfo = playerRankInfo;
}

void MtgaMatch::onPlayerRankUpdated(RankInfo playerCurrentRankInfo,
                                    RankInfo playerOldRankInfo, int seasonOrdinal)
{
    matchDetails.playerCurrentRankInfo = playerCurrentRankInfo;
    matchDetails.playerOldRankInfo = playerOldRankInfo;
    matchDetails.seasonOrdinal = seasonOrdinal;
}

void MtgaMatch::onPlayerMythicRatingUpdated(double oldMythicPercentile, double newMythicPercentile,
                                            int newMythicLeaderboardPlacement)
{
    matchDetails.playerCurrentRankInfo.setMythicInfo(newMythicPercentile, newMythicLeaderboardPlacement);
    matchDetails.playerOldRankInfo.setMythicInfo(oldMythicPercentile);
}

void MtgaMatch::onPlayerTakesMulligan(QMap<int, int> newHandDrawed)
{
    if (!isRunning) {
        return;
    }
    matchDetails.currentGame().playerMulligan = true;
    QMap<int, int> handObjectIds;
    for (MatchZone zone : gameZones.values()) {
        if (zone.type() == ZoneType_HAND && zone.ownerSeatId() == matchDetails.player.seatId()) {
            handObjectIds = zone.objectIds;
            break;
        }
    }
    for (int zoneId : gameZones.keys()) {
        MatchZone zone = gameZones[zoneId];
        if (zone.type() == ZoneType_LIBRARY && zone.ownerSeatId() == matchDetails.player.seatId()) {
            for (int objectId : handObjectIds.keys()) {
                zone.objectIds[objectId] = 0;
                Card* card = mtgCards->findCard(handObjectIds[objectId]);
                emit sgnPlayerPutOnLibraryCard(card);
            }
            gameZones[zoneId] = zone;
            break;
        }
    }
    for(int mtgaCardId : newHandDrawed.values()) {
        Card* card = mtgCards->findCard(mtgaCardId);
        LOGI(QString("Player draw %1").arg(card->name));
        emit sgnPlayerDrawCard(card);
    }
}

void MtgaMatch::onOpponentTakesMulligan(int opponentSeatId)
{
    UNUSED(opponentSeatId);
    if (!isRunning) {
        return;
    }
    matchDetails.currentGame().opponentMulligan = true;
}

void MtgaMatch::onMatchStateDiff(MatchStateDiff matchStateDiff)
{
    if (!isRunning) {
        return;
    }
    // Initial player hand draws
    for (MatchZone zone : gameZones) {
        if (zone.type() == ZoneType_HAND && zone.ownerSeatId() == matchDetails.player.seatId()) {
            if (zone.objectIds.size() == 0) {
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
        MatchZone zoneSrc = gameZones[idsZoneChanged[objectId].zoneSrcId()];
        MatchZone zoneDst = gameZones[idsZoneChanged[objectId].zoneDstId()];
        int oldObjectId = 0;
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
    // Analyse objects revealed
    QList<QPair<int, int>> revealedCardCreated = matchStateDiff.revealedCardCreated();
    for (QPair<int, int> revealedCardCreated : revealedCardCreated) {
        bool isPlayerCardRevealed = revealedCardCreated.second == matchDetails.player.seatId();
        if (!isPlayerCardRevealed) {
            continue;
        }
        int grpId = revealedCardCreated.first;
        if (this->revealedCards.keys().contains(grpId)) {
            this->revealedCards[grpId] += 1;
        } else {
            this->revealedCards.insert(grpId, 0);
        }
        Card* card = mtgCards->findCard(grpId);
        if (card->mtgaId > 0) {
            emit sgnPlayerRevealCard(card);
        }
    }
}

void MtgaMatch::notifyHandCardsDraw(MatchStateDiff matchStateDiff)
{
    for (MatchZone zone : matchStateDiff.zones()) {
        if (zone.type() == ZoneType_HAND && zone.ownerSeatId() == matchDetails.player.seatId()) {
            for(int mtgaCardId : zone.objectIds.values()) {
                Card* card = mtgCards->findCard(mtgaCardId);
                LOGI(QString("Player draw %1").arg(card->name));
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
            LOGI(QString("%1 draw %2").arg(ownerIdenfitier).arg(cardName));
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
            LOGI(QString("%1 %2 %3").arg(ownerIdenfitier).arg(action).arg(cardName));
            if (isTransferFromPlayer) {
                emitPlayerCardRevealSignal(card);
                emit sgnPlayerPlayCard(card);
            } else {
                emit sgnOpponentPlayCard(card);
            }
            break;
        }
        case TRANSFER_DESTROY: {
            LOGI(QString("%1 destroyed").arg(cardName));
            break;
        }
        case TRANSFER_DISCARD: {
            LOGI(QString("%1 discarded").arg(cardName));
            if (isTransferFromPlayer) {
                emitPlayerCardRevealSignal(card);
                emit sgnPlayerDiscardCard(card);
            } else {
                emit sgnOpponentDiscardCard(card);
            }
            break;
        }
        case TRANSFER_DISCARD_FROM_LIBRARY: {
            if (card->mtgaId == 0) {
                for (MatchZone zone : gameZones) {
                    if (zone.type() == ZoneType_LIMBO) {
                        card = getCardByObjectId(zone, objectId);
                        cardName = card ? card->name : QString("Object %1").arg(objectId);
                        break;
                    }
                }
            }
            LOGI(QString("%1 discarded from library").arg(cardName));
            if (card->mtgaId > 0) {
                if (isTransferFromPlayer) {
                    emitPlayerCardRevealSignal(card);
                    emit sgnPlayerDiscardFromLibraryCard(card);
                } else {
                    emit sgnOpponentDiscardFromLibraryCard(card);
                }
            }
            break;
        }
        case TRANSFER_EXILE: {
            LOGI(QString("%1 exiled").arg(cardName));
            break;
        }
        case TRANSFER_COUNTERED: {
            LOGI(QString("%1 countered").arg(cardName));
            break;
        }
        case TRANSFER_RESOLVE: {
            LOGD(QString("%1 resolved").arg(cardName));
            break;
        }
        case TRANSFER_PUT_ON_BATTLEFIELD: {
            LOGI(QString("%1 put %2 on battlefield").arg(ownerIdenfitier).arg(cardName));
            if (isTransferFromPlayer) {
                emitPlayerCardRevealSignal(card);
                emit sgnPlayerPutOnBattlefieldCard(card);
            } else {
                emit sgnOpponentPutOnBattlefieldCard(card);
            }
            break;
        }
        case TRANSFER_PUT_ON_TOP: {
            LOGI(QString("%1 put %2 on top of deck").arg(ownerIdenfitier).arg(cardName));
            break;
        }
        case TRANSFER_PUT_ON_LIBRARY: {
            LOGI(QString("%1 put %2 on library").arg(ownerIdenfitier).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerPutOnLibraryCard(card);
            } else {
                emit sgnOpponentPutOnLibraryCard(card);
            }
            break;
        }
        case TRANSFER_PUT_ON_HAND: {
            LOGI(QString("%1 put %2 on hand").arg(ownerIdenfitier).arg(cardName));
            if (isTransferFromPlayer) {
                emit sgnPlayerPutOnHandCard(card);
            } else {
                emit sgnOpponentPutOnHandCard(card);
            }
            break;
        }
        case TRANSFER_RETURN: {
            Card* returnedCard = nullptr;
            for (MatchZone zone : gameZones) {
                if (zone.type() == ZoneType_LIMBO) {
                    returnedCard = getCardByObjectId(zone, oldObjectId);
                    break;
                }
            }
            if (returnedCard) {
                LOGI(QString("%1 returned to hand").arg(returnedCard->name));
            } else {
                LOGI(QString("%1 returned to hand").arg(cardName));
            }
            break;
        }
        default: {
            LOGD(QString("*** %1 move %2 from %3 to %4").arg(ownerIdenfitier)
                 .arg(cardName).arg(zoneSrc.name()).arg(zoneDst.name()));
        }
    }
}

void MtgaMatch::emitPlayerCardRevealSignal(Card *card)
{
    if (this->revealedCards.keys().contains(card->mtgaId)) {
        int qtdRevealed = this->revealedCards[card->mtgaId];
        if (this->revealedCardsPlayed.keys().contains(card->mtgaId)) {
            if (this->revealedCardsPlayed[card->mtgaId] >= qtdRevealed) {
                emit sgnPlayerRevealCard(card);
            }
            this->revealedCardsPlayed[card->mtgaId] += 1;
        } else {
            this->revealedCardsPlayed.insert(card->mtgaId, 1);
        }
    } else {
        emit sgnPlayerRevealCard(card);
    }
}

void MtgaMatch::onNewTurnStarted(int turnNumber)
{
    if (!isRunning) {
        return;
    }
    currentTurn = turnNumber;
    LOGI(QString("Turn %1 started").arg(turnNumber));
}

void MtgaMatch::onSummarizedMessage()
{
    summarizedMessage++;
}

void MtgaMatch::onActivePlayer(int player)
{
    matchDetails.onActivePlayer(player);
}

void MtgaMatch::onDecisionPlayer(int player)
{
    matchDetails.onDecisionPlayer(player);
}

void MtgaMatch::updateZones(MatchStateDiff matchStateDiff)
{
    QMap<int, MatchZone> actualZones = gameZones;
    for (MatchZone zone : matchStateDiff.zones()) {
        gameZones[zone.id()] = zone;
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
        for (MatchZone zone : gameZones) {
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
    return new Card(cardId, 0, "", "", "", QString("UNKNOWN %1").arg(cardId));
}

QString MtgaMatch::getOwnerIdentifier(int objectId, MatchZone zoneSrc)
{
    int ownerId = zoneSrc.type() == ZoneType_STACK ? stackOwnerTrack[objectId]
              : zoneSrc.ownerSeatId();
    if (ownerId == matchDetails.player.seatId()) {
        return "Player";
    }
    if (ownerId == matchDetails.opponent.seatId()) {
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
    if (zoneSrc.type() != ZoneType_LIBRARY && zoneDst.type() == ZoneType_HAND) {
        return TRANSFER_PUT_ON_HAND;
    }
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_BATTLEFIELD) {
        return TRANSFER_PUT_ON_BATTLEFIELD;
    }
    if (zoneSrc.type() == ZoneType_LIBRARY && zoneDst.type() == ZoneType_LIBRARY) {
        return TRANSFER_PUT_ON_TOP;
    }
    if (zoneSrc.type() != ZoneType_LIBRARY && zoneDst.type() == ZoneType_LIBRARY) {
        return TRANSFER_PUT_ON_LIBRARY;
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
