#include "mtgalogparser.h"
#include "../lotustracker.h"
#include "../transformations.h"
#include "../macros.h"

#include <QList>
#include <QRegularExpression>

#define REGEXP_RAW_MSG "\\s(<==|Incoming|Match\\sto).+(\\s|\\n)(\\{|\\[)(\\n\\s+.*)+\\n(\\}|\\])"
#define REGEXP_MSG_RESPONSE_NUMBER "((?<=\\s)\\d+(?=\\:\\s)|(?<=\\()\\d+(?=\\)))"
#define REGEXP_MSG_ID "\\S+(?=(\\s|\\n)(\\{|\\[))"
#define REGEXP_MSG_JSON "(\\{|\\[)(\\n\\s+.*)+\\n(\\}||\\])"

MtgaLogParser::MtgaLogParser(QObject *parent, MtgCards *mtgCards)
    : QObject(parent), mtgCards(mtgCards)
{
    reRawMsg = QRegularExpression(REGEXP_RAW_MSG);
    reMsgNumber = QRegularExpression(REGEXP_MSG_RESPONSE_NUMBER);
    reMsgId = QRegularExpression(REGEXP_MSG_ID);
    reMsgJson = QRegularExpression(REGEXP_MSG_JSON);
}

MtgaLogParser::~MtgaLogParser()
{

}

Deck MtgaLogParser::jsonObject2Deck(QJsonObject jsonDeck)
{
    QString id = jsonDeck["id"].toString();
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
    QJsonArray jsonSideboard = jsonDeck["sideboard"].toArray();
    QMap<Card*, int> sideboard;
    for(QJsonValueRef jsonCardRef : jsonSideboard){
        QJsonObject jsonCard = jsonCardRef.toObject();
        int cardId = jsonCard["id"].toString().toInt();
        Card* card = mtgCards->findCard(cardId);
        if (card) {
            sideboard[card] = jsonCard["quantity"].toInt();
        }
    }
    return Deck(id, name, cards, sideboard);
}

void MtgaLogParser::parse(QString logNewContent)
{
    // Extract raw msgs
    QRegularExpressionMatchIterator iterator = reRawMsg.globalMatch(logNewContent);
    QList<QString> rawMsgs;
    while (iterator.hasNext()) {
        rawMsgs << iterator.next().captured(0);
    }
    // Extract msg id and json
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
            if (msgId.contains("(")) {
                msgId = msgId.left(msgId.indexOf("("));
            }
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
#ifdef QT_DEBUG
    if (msg.first != "GreToClientEvent") {
        LOGD(msg.first);
    }
#endif
    if (msg.first == "PlayerInventory.GetPlayerInventory") {
        parsePlayerInventory(msg.second);
    } else if (msg.first == "Inventory.Updated"){
        parsePlayerInventoryUpdate(msg.second);
    } else if (msg.first == "PlayerInventory.GetPlayerCardsV3"){
        parsePlayerCollection(msg.second);
    } else if (msg.first == "Deck.GetDeckLists"){
        parsePlayerDecks(msg.second);
    } else if (msg.first == "Deck.CreateDeck"){
        parsePlayerDeckCreate(msg.second);
    } else if (msg.first == "Deck.UpdateDeck"){
        parsePlayerDeckUpdate(msg.second);
    } else if (msg.first == "Event.GetPlayerCourse"){
        parseEventPlayerCourse(msg.second);
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
    int gold = jsonPlayerIventory["gold"].toInt();
    int gems = jsonPlayerIventory["gems"].toInt();
    int wcCommon = jsonPlayerIventory["wcCommon"].toInt();
    int wcUncommon = jsonPlayerIventory["wcUncommon"].toInt();
    int wcRare = jsonPlayerIventory["wcRare"].toInt();
    int wcMythic = jsonPlayerIventory["wcMythic"].toInt();
    double vaultProgress = jsonPlayerIventory["vaultProgress"].toDouble();
    PlayerInventory playerInventory(gold, gems, wcCommon, wcUncommon, wcRare, wcMythic, vaultProgress);
    LOGD(QString("PlayerInventory: %1 wcC, %2 wcI, %3 wcR, %4 wcM")
         .arg(wcCommon).arg(wcUncommon).arg(wcRare).arg(wcMythic));
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

void MtgaLogParser::parseEventPlayerCourse(QString json)
{
    QJsonObject jsonEventPlayerCourse = Transformations::stringToJsonObject(json);
    if (jsonEventPlayerCourse.empty()) {
        return;
    }
    QString eventId = jsonEventPlayerCourse["InternalEventName"].toString();
    QJsonObject jsonEventPlayerCourseDeck = jsonEventPlayerCourse["CourseDeck"].toObject();
    Deck deck = jsonObject2Deck(jsonEventPlayerCourseDeck);
    LOGD(QString("EventPlayerCourse: %1 with %2").arg(eventId).arg(deck.name));
    emit sgnEventPlayerCourse(eventId, deck);
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
    QString eventId = jsonMatchCreated["eventId"].toString();
    OpponentInfo opponentInfo(opponentName, opponentRankClass, opponentRankTier);
    LOGD(QString("MatchCreated: Opponent %1, rank: %2(%3)").arg(opponentName)
         .arg(opponentRankClass).arg(opponentRankTier));
    emit sgnMatchCreated(eventId, opponentInfo);
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
        int matchWinningTeamId = -1;
        for (QJsonValueRef jsonPlayerRef : jsonResults) {
            QJsonObject jsonResult = jsonPlayerRef.toObject();
            QString resultScope = jsonResult["scope"].toString();
            int winningTeamId = jsonResult["winningTeamId"].toInt();
            if (resultScope == "MatchScope_Match") {
                matchWinningTeamId = winningTeamId;
            }
        }
        LOGD("MatchInfoResult");
        emit sgnMatchResult(matchWinningTeamId);
    }
}

void MtgaLogParser::parsePlayerRankInfo(QString json)
{
    QJsonObject jsonPlayerRankInfo = Transformations::stringToJsonObject(json);
    if (jsonPlayerRankInfo.empty()) {
        return;
    }
    QString rankClass = jsonPlayerRankInfo["constructedClass"].toString();
    int rankTier = jsonPlayerRankInfo["constructedTier"].toInt();
    LOGD(QString("PlayerRankInfo: %1 - %2").arg(rankClass).arg(rankTier));
    emit sgnPlayerRankInfo(qMakePair(rankClass, rankTier));
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

void MtgaLogParser::parsePlayerDeckCreate(QString json)
{
    QJsonObject jsonPlayerCreateDeck = Transformations::stringToJsonObject(json);
    if (jsonPlayerCreateDeck.empty()) {
        return;
    }
    Deck deck = jsonObject2Deck(jsonPlayerCreateDeck);
    LOGD(QString("PlayerCreateDeck: %1").arg(deck.name));
    emit sgnPlayerDeckCreated(deck);
}

void MtgaLogParser::parsePlayerDeckUpdate(QString json)
{
    QJsonObject jsonPlayerUpdateDeck = Transformations::stringToJsonObject(json);
    if (jsonPlayerUpdateDeck.empty()) {
        return;
    }
    Deck deck = jsonObject2Deck(jsonPlayerUpdateDeck);
    LOGD(QString("PlayerUpdateDeck: %1").arg(deck.name));
    emit sgnPlayerDeckUpdated(deck);
}

void MtgaLogParser::parsePlayerDeckSubmited(QString json)
{
    QJsonObject jsonPlayerDeckSubmited = Transformations::stringToJsonObject(json);
    if (jsonPlayerDeckSubmited.empty()) {
        return;
    }
    QString eventId = jsonPlayerDeckSubmited["InternalEventName"].toString();
    QJsonObject jsonDeck = jsonPlayerDeckSubmited["CourseDeck"].toObject();
    Deck deckSubmited = jsonObject2Deck(jsonDeck);
    LOGD(QString("Deck submited: %1").arg(deckSubmited.name));
    emit sgnPlayerDeckSubmited(eventId, deckSubmited);
}

void MtgaLogParser::parseSubmitDeck(QJsonObject jsonMessage)
{
    QJsonObject jsonDeck = jsonMessage["submitDeckReq"].toObject();
    QJsonArray jsonCards = jsonDeck["deck"].toObject()["deckCards"].toArray();
    QMap<Card*, int> cards;
    for(QJsonValueRef jsonCardRef : jsonCards){
        int cardId = jsonCardRef.toInt();
        Card* card = mtgCards->findCard(cardId);
        if (cards.keys().contains(card)) {
            cards[card] += 1;
        } else {
            cards[card] = 1;
        }
    }
    emit sgnPlayerDeckWithSideboardSubmited(cards);
}

void MtgaLogParser::parseGreToClientMessages(QString json)
{
    QJsonObject jsonPlayerToClientMsg = Transformations::stringToJsonObject(json);
    if (jsonPlayerToClientMsg.empty()) {
        return;
    }
    QJsonObject jsonGreToClientEvent = jsonPlayerToClientMsg["greToClientEvent"].toObject();
    QJsonArray jsonGreToClientMessages = jsonGreToClientEvent["greToClientMessages"].toArray();
    bool hasMulliganReq = false;
    for (QJsonValueRef jsonMessageRef : jsonGreToClientMessages) {
        QJsonObject jsonMessage = jsonMessageRef.toObject();
        QString messageType = jsonMessage["type"].toString();
        if (messageType == "GREMessageType_MulliganReq") {
            hasMulliganReq = true;
        }
    }
    for (QJsonValueRef jsonMessageRef : jsonGreToClientMessages) {
        QJsonObject jsonMessage = jsonMessageRef.toObject();
        QString messageType = jsonMessage["type"].toString();
        if (messageType == "GREMessageType_SubmitDeckReq") {
            parseSubmitDeck(jsonMessage);
        }
        if (messageType == "GREMessageType_GameStateMessage" ||
                   messageType == "GREMessageType_QueuedGameStateMessage") {
            int gameStateId = jsonMessage["gameStateId"].toInt();
            QJsonObject jsonGameStateMessage = jsonMessage["gameStateMessage"].toObject();
            QString gameStateType = jsonGameStateMessage["type"].toString();
            if (gameStateType == "GameStateType_Full") {
                parseGameStateFull(jsonGameStateMessage);
            } else if (gameStateType == "GameStateType_Diff") {
                parseGameStateDiff(gameStateId, jsonGameStateMessage, hasMulliganReq);
            }
        }
    }
}

void MtgaLogParser::parseGameStateFull(QJsonObject jsonMessage)
{
    QJsonObject gameInfo = jsonMessage["gameInfo"].toObject();
    QString winCondition = gameInfo["matchWinCondition"].toString();
    MatchMode mode = MatchMode_UNKNOWN;
    if (winCondition == "MatchWinCondition_SingleElimination") {
        mode = MatchMode_SINGLE;
    }
    if (winCondition == "MatchWinCondition_Best2of3") {
        mode = MatchMode_BEST_OF_3;
    }
    QList<MatchZone> zones = getMatchZones(jsonMessage);
    int seatId = jsonMessage["turnInfo"].toObject()["decisionPlayer"].toInt();
    LOGD(QString("GameStart Mode: %1, Zones: %2, DecisionPlayer: %3")
         .arg(MatchInfo::MatchModeToString(mode)).arg(zones.size()).arg(seatId));
    emit sgnGameStart(mode, zones, seatId);
}

void MtgaLogParser::parseGameStateDiff(int gameStateId, QJsonObject jsonMessage, bool hasMulliganReq)
{
    QJsonObject jsonInfoObject = jsonMessage["gameInfo"].toObject();
    if (jsonInfoObject["matchState"].toString() == "MatchState_GameComplete") {
        QJsonArray jsonResults = jsonInfoObject["results"].toArray();
        emit sgnGameCompleted(getGameResults(jsonResults));
    }
    QList<MatchZone> zones = getMatchZones(jsonMessage);
    if (hasMulliganReq) {
        checkPlayerMulligan(zones);
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
    int turnNumber = 0;
    QJsonArray jsonGSMAnnotations = jsonMessage["annotations"].toArray();
    for (QJsonValueRef jsonAnnotationRef : jsonGSMAnnotations) {
        QJsonObject jsonAnnotation = jsonAnnotationRef.toObject();
        QString type = jsonAnnotation["type"].toArray().first().toString();
        if (type == "AnnotationType_NewTurnStarted") {
            QJsonObject jsonTurnInfo = jsonMessage["turnInfo"].toObject();
            if (jsonTurnInfo.contains("turnNumber")) {
                turnNumber = jsonTurnInfo["turnNumber"].toInt();
                LOGD(QString("NewTurn: %1").arg(turnNumber));
                emit sgnNewTurnStarted(turnNumber);
            }
        }
    }
    QJsonArray jsonDiffDeletedInstanceIds = jsonMessage["diffDeletedInstanceIds"].toArray();
    checkOpponentMulligan(zones, turnNumber, jsonDiffDeletedInstanceIds);
    QMap<int, int> idsChanged = getIdsChanged(jsonGSMAnnotations);
    QMap<int, MatchZoneTransfer> idsZoneChanged = getIdsZoneChanged(jsonGSMAnnotations);
    MatchStateDiff matchStateDiff(gameStateId, zones, idsChanged, idsZoneChanged);
    emit sgnMatchStateDiff(matchStateDiff);
}

QMap<int, int> MtgaLogParser::getGameResults(QJsonArray jsonResults)
{
    QMap<int, int> teamIdWins;
    for (QJsonValueRef jsonPlayerRef : jsonResults) {
        QJsonObject jsonResult = jsonPlayerRef.toObject();
        QString resultScope = jsonResult["scope"].toString();
        int winningTeamId = jsonResult["winningTeamId"].toInt();
        if (resultScope == "MatchScope_Game") {
            if (teamIdWins.keys().contains(winningTeamId)) {
                teamIdWins[winningTeamId] += 1;
            } else {
                teamIdWins[winningTeamId] = 1;
            }
        }
    }
    return teamIdWins;
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

void MtgaLogParser::checkPlayerMulligan(QList<MatchZone> zones)
{
    int libraryPlusHandSize = 0;
    for (MatchZone zone : zones) {
        if (zone.type() == ZoneType_LIBRARY) {
            libraryPlusHandSize += zone.objectIds.size();
        }
        if (zone.type() == ZoneType_HAND) {
            if (zone.objectIds.size() == 0 || zone.objectIds.size() == 7) {
                return;
            }
            libraryPlusHandSize += zone.objectIds.size();
        }
    }
    if (libraryPlusHandSize >= 60) {
        LOGD("Player mulligan");
        emit sgnPlayerTakesMulligan();
    }
}

void MtgaLogParser::checkOpponentMulligan(QList<MatchZone> zones, int turnNumber,
                                          QJsonArray jsonDiffDeletedInstanceIds)
{
    if (turnNumber > 1) {
        return;
    }
    QList<int> diffDeletedInstanceIds;
    for (QJsonValueRef jsonDiffDeletedInstanceIdsRef : jsonDiffDeletedInstanceIds) {
        diffDeletedInstanceIds << jsonDiffDeletedInstanceIdsRef.toInt();
    }
    if (diffDeletedInstanceIds.size() < 60) {
        return;
    }
    MatchZone opponentHandZone;
    for (MatchZone zone : zones) {
        if (zone.type() == ZoneType_HAND && !zone.hasKnownObjectIds()) {
            opponentHandZone = zone;
            break;
        }
    }
    if (opponentHandZone.id() == 0 || opponentHandZone.objectIds.size() == 7) {
        return;
    }
    MatchZone opponentLibraryZone;
    for (MatchZone zone : zones) {
        if (zone.type() == ZoneType_LIBRARY && zone.ownerSeatId() == opponentHandZone.ownerSeatId()) {
            opponentLibraryZone = zone;
            break;
        }
    }
    if (opponentHandZone.objectIds.size() + opponentLibraryZone.objectIds.size() < 60) {
        return;
    }
    LOGD("OpponentTakesMulligan");
    int opponentSeatId = zones[0].ownerSeatId();
    emit sgnOpponentTakesMulligan(opponentSeatId);
}

QMap<int, int> MtgaLogParser::getIdsChanged(QJsonArray jsonGSMAnnotations)
{
    QMap<int, int> idsChanged;
    for (QJsonValueRef jsonAnnotationRef : jsonGSMAnnotations) {
        QJsonObject jsonAnnotation = jsonAnnotationRef.toObject();
        QString type = jsonAnnotation["type"].toArray().first().toString();
        if (type == "AnnotationType_ObjectIdChanged") {
            QJsonArray jsonDetails = jsonAnnotation["details"].toArray();
            int orgId, newId = 0;
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
