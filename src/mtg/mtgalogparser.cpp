#include "mtgalogparser.h"
#include "../arenatracker.h"
#include "../transformations.h"
#include "../macros.h"

#include <QList>
#include <QRegularExpression>

#define REGEXP_RAW_MSG "\\s(Response|Incoming|Match\\sto|to\\sMatch).+(\\s|\\n)(\\{|\\[)(\\n\\s+.*)+\\n(\\}|\\])"
#define REGEXP_MSG_RESPONSE_NUMBER "(?<=\\s)\\d+(?=\\:\\s)"
#define REGEXP_MSG_ID "\\S+(?=(\\s|\\n)(\\{|\\[))"
#define REGEXP_MSG_JSON "(\\{|\\[)(\\n\\s+.*)+\\n(\\}||\\])"

MtgaLogParser::MtgaLogParser(QObject *parent, MtgCards *mtgCards)
    : QObject(parent), mtgCards(mtgCards)
{

}

MtgaLogParser::~MtgaLogParser()
{

}

Deck MtgaLogParser::jsonObject2Deck(QJsonObject jsonDeck)
{
    QString name = jsonDeck["name"].toString();
    QJsonArray jsonCards = jsonDeck["mainDeck"].toArray();
    QMap<Card*, int> cards;
    for(QJsonValueRef jsonCardRef : jsonCards){
        QJsonObject jsonCard = jsonCardRef.toObject();
        int cardId = jsonCard["id"].toString().toInt();
        Card* card = mtgCards->findCard(cardId);
        if (card) {
            cards[card] = jsonCard["quantity"].toInt();
        }
    }
    return Deck(name, cards);
}

void MtgaLogParser::parse(QString logNewContent)
{
    // Extract raw msgs
    QRegularExpression reRawMsg(REGEXP_RAW_MSG);
    QRegularExpressionMatchIterator iterator = reRawMsg.globalMatch(logNewContent);
    QList<QString> rawMsgs;
    while (iterator.hasNext()) {
        rawMsgs << iterator.next().captured(0);
    }
    // Extract msg id and json
    QRegularExpression reMsgNumber(REGEXP_MSG_RESPONSE_NUMBER);
    QRegularExpression reMsgId(REGEXP_MSG_ID);
    QRegularExpression reMsgJson(REGEXP_MSG_JSON);
    // List of msgs in format (msgId, msgJson)
    QList<QPair<QString, QString>> msgs;
    for (QString msg: rawMsgs) {
        QRegularExpressionMatch numberMatch = reMsgNumber.match(msg);
        if (numberMatch.hasMatch()) {
            int msgNumber = numberMatch.captured(0).toInt();
            // Avoid process duplicated responses
            if (msgResponseNumbers.contains(msgNumber)) {
                continue;
            } else {
                msgResponseNumbers << msgNumber;
                // Store only last 4 msg resposne number
                if (msgResponseNumbers.size() >= 5) {
                    msgResponseNumbers.removeFirst();
                }
            }
        }
        QString msgId = "";
        QRegularExpressionMatch idMatch = reMsgId.match(msg);
        if (idMatch.hasMatch()) {
            msgId = idMatch.captured(0);
        }
        QString msgJson = "";
        QRegularExpressionMatch jsonMatch = reMsgJson.match(msg);
        if (jsonMatch.hasMatch()) {
            msgJson = jsonMatch.captured(0);
            if (msgJson.at(0) == '[' && msgJson.right(1) != ']') {
                msgJson += "]";
            }
        }
        msgs << QPair<QString, QString>(msgId, msgJson);
    }
    // Log msgs
    for (QPair<QString, QString> msg: msgs) {
        parseMsg(msg);
    }
}

void MtgaLogParser::parseMsg(QPair<QString, QString> msg)
{
    if (msg.first == "PlayerInventory.GetPlayerInventory") {
        parsePlayerInventory(msg.second);
    } else if (msg.first == "Inventory.Updated"){
        parsePlayerInventoryUpdate(msg.second);
    } else if (msg.first == "PlayerInventory.GetPlayerCardsV3"){
        parsePlayerCollection(msg.second);
    } else if (msg.first == "Deck.GetDeckLists"){
        parsePlayerDecks(msg.second);
    } else if (msg.first == "Event.MatchCreated"){
        parseMatchCreated(msg.second);
    } else if (msg.first == "MatchGameRoomStateChangedEvent"){
        parseMatchInfo(msg.second);
    } else if (msg.first == "Event.GetCombinedRankInfo"){
        parsePlayerRankInfo(msg.second);
    } else if (msg.first == "Rank.Updated"){
        parsePlayerRankUpdated(msg.second);
    } else if (msg.first == "Event.DeckSubmit"){
        parsePlayerDeckSubmited(msg.second);
    } else if (msg.first == "ClientToGreMessage"){
        parseClientToGreMessages(msg.second);
    } else if (msg.first == "GreToClientEvent"){
        parseGreToClientMessages(msg.second);
    }
}

void MtgaLogParser::parsePlayerInventory(QString json)
{
    QJsonObject jsonPlayerIventory = Transformations::stringToJsonObject(json);
    if (jsonPlayerIventory.empty()) {
        return;
    }
    int wcCommon = jsonPlayerIventory["wcCommon"].toInt();
    int wcUncommon = jsonPlayerIventory["wcUncommon"].toInt();
    int wcRare = jsonPlayerIventory["wcRare"].toInt();
    int wcMythic = jsonPlayerIventory["wcMythic"].toInt();
    float vaultProgress = jsonPlayerIventory["vaultProgress"].toDouble();
    PlayerInventory playerInventory(wcCommon, wcUncommon, wcRare, wcMythic, vaultProgress);
    LOGD(QString("PlayerInventory: %1 wcC, %2 wcR, %3 wcM, %4% vault")
         .arg(wcCommon).arg(wcUncommon).arg(wcRare).arg(wcMythic).arg(vaultProgress));
    emit sgnPlayerInventory(playerInventory);
}

void MtgaLogParser::parsePlayerInventoryUpdate(QString json)
{
    QJsonObject jsonPlayerIventoryUpdate = Transformations::stringToJsonObject(json);
    if (jsonPlayerIventoryUpdate.empty()) {
        return;
    }
    QJsonObject delta = jsonPlayerIventoryUpdate["delta"].toObject();
    QJsonArray jsonCards = delta["cardsAdded"].toArray();

    QList<int> newCards;
    for (QJsonValueRef jsonCardRef: jsonCards) {
        int newCardId = jsonCardRef.toInt();
        newCards << newCardId;
    }
    LOGD(QString("PlayerInventoryUpdate: %1 new cards").arg(newCards.size()));
    emit sgnPlayerInventoryUpdate(newCards);
}

void MtgaLogParser::parsePlayerCollection(QString json)
{
    QJsonObject jsonPlayerCollection = Transformations::stringToJsonObject(json);
    if (jsonPlayerCollection.empty()) {
        return;
    }
    QMap<int, int> ownedCards;
    for (QString ownedCardId: jsonPlayerCollection.keys()) {
        int ownedCardQtd = jsonPlayerCollection[ownedCardId].toInt();
        ownedCards[ownedCardId.toInt()] = ownedCardQtd;
    }
    LOGD(QString("PlayerCollection: %1 unique cards").arg(ownedCards.size()));
    emit sgnPlayerCollection(ownedCards);
}

void MtgaLogParser::parsePlayerDecks(QString json)
{
    QJsonArray jsonPlayerDecks = Transformations::stringToJsonArray(json);
    if (jsonPlayerDecks.empty()) {
        return;
    }
    QList<Deck> playerDecks;
    for (QJsonValueRef jsonDeckRef: jsonPlayerDecks) {
        QJsonObject jsonDeck = jsonDeckRef.toObject();
        playerDecks << jsonObject2Deck(jsonDeck);
    }
    LOGD(QString("PlayerDecks: %1 decks").arg(playerDecks.size()));
    emit sgnPlayerDecks(playerDecks);
}

void MtgaLogParser::parseMatchCreated(QString json)
{
    QJsonObject jsonMatchCreated = Transformations::stringToJsonObject(json);
    if (jsonMatchCreated.empty()) {
        return;
    }
    QString opponentName = jsonMatchCreated["opponentScreenName"].toString();
    QString opponentRankClass = jsonMatchCreated["opponentRankingClass"].toString();
    int opponentRankTier = jsonMatchCreated["opponentRankingTier"].toInt();
    MatchInfo match(opponentName, opponentRankClass, opponentRankTier);
    LOGD(QString("MatchCreated: Opponent %1, rank: %2 - %3").arg(opponentName)
         .arg(opponentRankClass).arg(opponentRankTier));
    emit sgnMatchCreated(match);
}

void MtgaLogParser::parseMatchInfo(QString json)
{
    QJsonObject jsonMatchInfo = Transformations::stringToJsonObject(json);
    if (jsonMatchInfo.empty()) {
        return;
    }
    QJsonObject jsonMatchState = jsonMatchInfo["matchGameRoomStateChangedEvent"].toObject();
    QJsonObject jsonRoomInfo = jsonMatchState["gameRoomInfo"].toObject();
    QString roomState = jsonRoomInfo["stateType"].toString();
    if (roomState == "MatchGameRoomStateType_Playing"){
        QJsonObject jsonRoomConfig = jsonRoomInfo["gameRoomConfig"].toObject();
        QJsonArray jsonPlayers = jsonRoomConfig["reservedPlayers"].toArray();
        QList<MatchPlayer> matchPlayers;
        for (QJsonValueRef jsonPlayerRef : jsonPlayers) {
            QJsonObject jsonPlayer = jsonPlayerRef.toObject();
            QString playerName = jsonPlayer["playerName"].toString();
            int playerSeat = jsonPlayer["systemSeatId"].toInt();
            int playerTeamId = jsonPlayer["teamId"].toInt();
            matchPlayers << MatchPlayer(playerName, playerSeat, playerTeamId);
        }
        LOGD("MatchInfoSeats");
        emit sgnMatchInfoSeats(matchPlayers);
    }
    if (roomState == "MatchGameRoomStateType_MatchCompleted"){
        QJsonObject jsonFinalMatchResult = jsonRoomInfo["finalMatchResult"].toObject();
        QJsonArray jsonResults = jsonFinalMatchResult["resultList"].toArray();
        int matchWinningTeamId;
        for (QJsonValueRef jsonPlayerRef : jsonResults) {
            QJsonObject jsonResult = jsonPlayerRef.toObject();
            QString resultScope = jsonResult["scope"].toString();
            if (resultScope == "MatchScope_Match") {
                matchWinningTeamId = jsonResult["winningTeamId"].toInt();
            }
        }
        LOGD("MatchInfoResult");
        emit sgnMatchInfoResultMatch(matchWinningTeamId);
    }
}

void MtgaLogParser::parsePlayerRankInfo(QString json)
{
    QJsonObject jsonPlayerRankInfo = Transformations::stringToJsonObject(json);
    if (jsonPlayerRankInfo.empty()) {
        return;
    }
    QJsonObject jsonConstructed = jsonPlayerRankInfo["constructed"].toObject();
    QString rankClass = jsonConstructed["class"].toString();
    int rankTier = jsonConstructed["tier"].toInt();
    int wins = jsonConstructed["wins"].toInt();
    int losses = jsonConstructed["losses"].toInt();
    LOGD(QString("PlayerRankInfo: %1 - %2, Status Wins: %3 Losses: %4")
         .arg(rankClass).arg(rankTier).arg(wins).arg(losses));
    emit sgnPlayerRankInfo(qMakePair(rankClass, rankTier));
    emit sgnPlayerRankStatus(qMakePair(wins, losses));
}

void MtgaLogParser::parsePlayerRankUpdated(QString json)
{
    QJsonObject jsonPlayerRankUpdate = Transformations::stringToJsonObject(json);
    if (jsonPlayerRankUpdate.empty()) {
        return;
    }
    int oldRankTier = jsonPlayerRankUpdate["oldTier"].toInt();
    int newRankTier = jsonPlayerRankUpdate["newTier"].toInt();
    if (newRankTier != oldRankTier) {
        QString rankClass = jsonPlayerRankUpdate["newClass"].toString();
        LOGD(QString("RankUpdate: new rank %1").arg(rankClass));
        emit sgnPlayerRankUpdated(qMakePair(rankClass, newRankTier));
    }
}

void MtgaLogParser::parsePlayerDeckSubmited(QString json)
{
    QJsonObject jsonPlayerDeckSubmited = Transformations::stringToJsonObject(json);
    if (jsonPlayerDeckSubmited.empty()) {
        return;
    }
    QJsonObject jsonDeck = jsonPlayerDeckSubmited["CourseDeck"].toObject();
    Deck deckSubmited = jsonObject2Deck(jsonDeck);
    LOGD(QString("Deck submited: %1").arg(deckSubmited.name));
    emit sgnPlayerDeckSubmited(deckSubmited);
}

void MtgaLogParser::parseClientToGreMessages(QString json)
{
    QJsonObject jsonPlayerToClientMsg = Transformations::stringToJsonObject(json);
    if (jsonPlayerToClientMsg.empty()) {
        return;
    }
    QJsonObject jsonClientToGreMsg = jsonPlayerToClientMsg["clientToGreMessage"].toObject();
    if (jsonClientToGreMsg.contains("uiMessage")) {
        QJsonObject jsonUiMessage = jsonClientToGreMsg["uiMessage"].toObject();
        QJsonObject jsonOnHover = jsonUiMessage["onHover"].toObject();
        if (jsonOnHover.contains("objectId")) {
            emit sgnPlayerCardHoverStarts();
        } else  {
            emit sgnPlayerCardHoverEnds();
        }
    }
    if (jsonClientToGreMsg.contains("mulliganResp")) {
        QJsonObject jsonMulliganResp = jsonClientToGreMsg["mulliganResp"].toObject();
        QString action = jsonMulliganResp["decision"].toString();
        if (action == "MulliganOption_AcceptHand") {
            LOGD("Player AcceptHand");
            emit sgnPlayerAcceptsHand();
        } else if (action == "MulliganOption_Mulligan") {
            LOGD("Player mulligan");
            emit sgnPlayerTakesMulligan();
        }
    }
}

void MtgaLogParser::parseGreToClientMessages(QString json)
{
    QJsonObject jsonPlayerToClientMsg = Transformations::stringToJsonObject(json);
    if (jsonPlayerToClientMsg.empty()) {
        return;
    }
    QJsonObject jsonGreToClientEvent = jsonPlayerToClientMsg["greToClientEvent"].toObject();
    QJsonArray jsonGreToClientMessages = jsonGreToClientEvent["greToClientMessages"].toArray();
    for (QJsonValueRef jsonMessageRef : jsonGreToClientMessages) {
        QJsonObject jsonMessage = jsonMessageRef.toObject();
        QString messageType = jsonMessage["type"].toString();
        if (messageType == "GREMessageType_DieRollResultsResp") {
            parseDieRollResult(jsonMessage);
        } else if (messageType == "GREMessageType_GameStateMessage" ||
                   messageType == "GREMessageType_QueuedGameStateMessage") {
            int gameStateId = jsonMessage["gameStateId"].toInt();
            QJsonObject jsonGameStateMessage = jsonMessage["gameStateMessage"].toObject();
            QString gameStateType = jsonGameStateMessage["type"].toString();
            if (gameStateType == "GameStateType_Full") {
                parseGameStateFull(jsonGameStateMessage);
            } else if (gameStateType == "GameStateType_Diff") {
                parseGameStateDiff(gameStateId, jsonGameStateMessage);
            }
        }
    }
}

void MtgaLogParser::parseDieRollResult(QJsonObject jsonMessage)
{
    QJsonObject jsonDieRollResultsResp = jsonMessage["dieRollResultsResp"].toObject();
    QJsonArray jsonPlayerDieRolls = jsonDieRollResultsResp["playerDieRolls"].toArray();
    QPair<int, int> highRollValueAndSeatId = qMakePair(0, 0);
    for (QJsonValueRef jsonPlayerDieRollsRef : jsonPlayerDieRolls) {
        QJsonObject jsonPlayerDieRoll = jsonPlayerDieRollsRef.toObject();
        int playerDieRollValue = jsonPlayerDieRoll["rollValue"].toInt();
        if (playerDieRollValue > highRollValueAndSeatId.first) {
            int playerSeatId = jsonPlayerDieRoll["systemSeatId"].toInt();
            highRollValueAndSeatId = qMakePair(playerDieRollValue, playerSeatId);
        }
    }
    if (highRollValueAndSeatId.first > 0) {
        LOGD(QString("SeatIdThatGoFirst: %1").arg(highRollValueAndSeatId.second))
        emit sgnSeatIdThatGoFirst(highRollValueAndSeatId.second);
    }
}

void MtgaLogParser::parseGameStateFull(QJsonObject jsonMessage)
{
    QList<MatchZone> zones = getMatchZones(jsonMessage);
    LOGD(QString("MatchStartZones: %1").arg(zones.size()));
    emit sgnMatchStartZones(zones);
}

void MtgaLogParser::parseGameStateDiff(int gameStateId, QJsonObject jsonMessage)
{
    QList<MatchZone> zones = getMatchZones(jsonMessage);
    for (MatchZone zone : zones) {
        if (zone.type() == ZoneType_HAND && zone.objectIds.isEmpty()) {
            LOGD("OpponentTakesMulligan");
            emit sgnOpponentTakesMulligan(zone.ownerSeatId());
        }
    }
    QJsonArray jsonGameObjects = jsonMessage["gameObjects"].toArray();
    for (QJsonValueRef jsonGameObjectRef : jsonGameObjects) {
        QJsonObject jsonGameObject = jsonGameObjectRef.toObject();
        int instanceId = jsonGameObject["instanceId"].toInt();
        int grpId = jsonGameObject["grpId"].toInt();
        int zoneId = jsonGameObject["zoneId"].toInt();
        for (MatchZone &zone : zones) {
            if (zone.id() == zoneId) {
                zone.objectIds[instanceId] = grpId;
                break;
            }
        }
    }
    QJsonArray jsonGSMAnnotations = jsonMessage["annotations"].toArray();
    for (QJsonValueRef jsonAnnotationRef : jsonGSMAnnotations) {
        QJsonObject jsonAnnotation = jsonAnnotationRef.toObject();
        QString type = jsonAnnotation["type"].toArray().first().toString();
        if (type == "AnnotationType_NewTurnStarted") {
            QJsonObject jsonTurnInfo = jsonMessage["turnInfo"].toObject();
            if (jsonTurnInfo.contains("turnNumber")) {
                int turnNumber = jsonTurnInfo["turnNumber"].toInt();
                LOGD(QString("NewTurn: %1").arg(turnNumber));
                emit sgnNewTurnStarted(turnNumber);
            }
        }
    }
    QMap<int, int> idsChanged = getIdsChanged(jsonGSMAnnotations);
    QMap<int, MatchZoneTransfer> idsZoneChanged = getIdsZoneChanged(jsonGSMAnnotations);
    MatchStateDiff matchStateDiff(gameStateId, zones, idsChanged, idsZoneChanged);
    emit sgnMatchStateDiff(matchStateDiff);
}

QList<MatchZone> MtgaLogParser::getMatchZones(QJsonObject jsonGameStateMessage)
{
    QJsonArray jsonZones = jsonGameStateMessage["zones"].toArray();
    QList<MatchZone> zones;
    for (QJsonValueRef jsonZoneRef : jsonZones) {
        QJsonObject jsonZone = jsonZoneRef.toObject();
        QString zoneTypeName = jsonZone["type"].toString();
        ZoneType zoneType = MatchZone::zoneTypeFromName(zoneTypeName);
        if (zoneType != ZoneType_UNKNOWN) {
            int id = jsonZone["zoneId"].toInt();
            int ownerSeatId = jsonZone["ownerSeatId"].toInt();
            QMap<int, int> objectIds;
            QJsonArray jsonObjectInstanceIds = jsonZone["objectInstanceIds"].toArray();
            for (QJsonValueRef jsonObjectIdRef : jsonObjectInstanceIds) {
                int objectId = jsonObjectIdRef.toInt();
                objectIds[objectId] = 0;
            }
            zones << MatchZone(id, ownerSeatId, zoneType, objectIds);
        }
    }
    return zones;
}

QMap<int, int> MtgaLogParser::getIdsChanged(QJsonArray jsonGSMAnnotations)
{

    QMap<int, int> idsChanged;
    for (QJsonValueRef jsonAnnotationRef : jsonGSMAnnotations) {
        QJsonObject jsonAnnotation = jsonAnnotationRef.toObject();
        QString type = jsonAnnotation["type"].toArray().first().toString();
        if (type == "AnnotationType_ObjectIdChanged") {
            QJsonArray jsonDetails = jsonAnnotation["details"].toArray();
            int orgId, newId;
            for (QJsonValueRef jsonDetailsRef : jsonDetails) {
                QJsonObject details = jsonDetailsRef.toObject();
                QString key = details["key"].toString();
                if (key == "orig_id") {
                    orgId = details["valueInt32"].toArray().first().toInt();
                } else if (key == "new_id") {
                    newId = details["valueInt32"].toArray().first().toInt();
                }
            }
            idsChanged[orgId] = newId;
        }
    }
    return idsChanged;
}

QMap<int, MatchZoneTransfer> MtgaLogParser::getIdsZoneChanged(QJsonArray jsonGSMAnnotations)
{
    QMap<int, MatchZoneTransfer> idsZoneChanged;
    for (QJsonValueRef jsonAnnotationRef : jsonGSMAnnotations) {
        QJsonObject jsonAnnotation = jsonAnnotationRef.toObject();
        QString type = jsonAnnotation["type"].toArray().first().toString();
        if (type == "AnnotationType_ZoneTransfer") {
            int transferId = jsonAnnotation["affectedIds"].toArray().first().toInt();
            QJsonArray jsonDetails = jsonAnnotation["details"].toArray();
            int srcZone, dstZone;
            ZoneTransferCategory transferCategory;
            for (QJsonValueRef jsonDetailsRef : jsonDetails) {
                QJsonObject details = jsonDetailsRef.toObject();
                QString key = details["key"].toString();
                if (key == "zone_src") {
                    srcZone = details["valueInt32"].toArray().first().toInt();
                } else if (key == "zone_dest") {
                    dstZone = details["valueInt32"].toArray().first().toInt();
                } else if (key == "category") {
                    QString category = details["valueString"].toArray().first().toString();
                    transferCategory = category == "Resolve" ? ZoneTransfer_RESOLVED
                                     : category == "Countered" ? ZoneTransfer_COUNTERED : ZoneTransfer_UNKOWN;
                }
            }
            idsZoneChanged[transferId] = MatchZoneTransfer(srcZone, dstZone, transferCategory);
        }
    }
    return idsZoneChanged;
}
