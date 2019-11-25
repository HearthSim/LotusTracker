#include "mtgalogparser.h"
#include "../lotustracker.h"
#include "../transformations.h"
#include "../macros.h"

#include <QList>
#include <QRegularExpression>

#define REGEXP_RAW_MSG "\\[UnityCrossThreadLogger\\][\\d\\w\\/]*(==>|to\\sMatch|<==|\\s|Match\\sto).+(\\s|\\n)[{\\[]([,:{}\\[\\]0-9.\\-+Eaeflnr-u\\s\\n\\r\\t]|\".*?\")+[}\\]]\\n"
#define REGEXP_MSG_ID "[\\w\\.]+(?=(\\(|((\\s|\\n)(\\{|\\[))))"
#define REGEXP_MSG_JSON "[{\\[]([,:{}\\[\\]0-9.\\-+Eaeflnr-u\\s\\n\\r\\t]|\".*?\")+[}\\]]\\n"

MtgaLogParser::MtgaLogParser(QObject *parent, MtgCards *mtgCards)
    : QObject(parent), matchRunning(false), mtgCards(mtgCards)
{
    reRawMsg = QRegularExpression(REGEXP_RAW_MSG);
    reMsgId = QRegularExpression(REGEXP_MSG_ID);
    reMsgJson = QRegularExpression(REGEXP_MSG_JSON);
}

MtgaLogParser::~MtgaLogParser()
{

}

Deck MtgaLogParser::jsonObject2DeckV1(QJsonObject jsonDeck)
{
    QString id = jsonDeck["id"].toString();
    QString name = jsonDeck["name"].toString();
    QJsonArray jsonCards = jsonDeck["mainDeck"].toArray();
    QMap<Card*, int> cards;
    for(QJsonValueRef jsonCardRef : jsonCards){
        QJsonObject value = jsonCardRef.toObject();
        int cardId = value["id"].toInt();
        Card* card = mtgCards->findCard(cardId);
        int qtd = value["quantity"].toInt();
        if (card && qtd > 0) {
            cards[card] = qtd;
        }
    }
    QJsonArray jsonSideboard = jsonDeck["sideboard"].toArray();
    QMap<Card*, int> sideboard;
    for(QJsonValueRef jsonCardRef : jsonSideboard){
        QJsonObject value = jsonCardRef.toObject();
        int cardId = value["id"].toInt();
        Card* card = mtgCards->findCard(cardId);
        int qtd = value["quantity"].toInt();
        if (card && qtd > 0) {
            sideboard[card] = qtd;
        }
    }
    int deckTileId = jsonDeck["deckTileId"].toInt();
    QList<QPair<int, QString>> cardSkins;
    QJsonArray jsonCardSkins = jsonDeck["cardSkins"].toArray();
    for(QJsonValueRef jsonCardSkinRef : jsonCardSkins){
        QJsonObject cardSkin = jsonCardSkinRef.toObject();
        int grpId = cardSkin["grpId"].toInt();
        QString ccv = cardSkin["ccv"].toString();
        cardSkins << qMakePair(grpId, ccv);
    }
    return Deck(id, name, cards, sideboard, deckTileId, cardSkins);
}

Deck MtgaLogParser::jsonObject2DeckV3(QJsonObject jsonDeck)
{
    QString id = jsonDeck["id"].toString();
    QString name = jsonDeck["name"].toString();
    QJsonArray jsonCards = jsonDeck["mainDeck"].toArray();
    QMap<Card*, int> cards = v3JsonArray2List(jsonCards);
    QJsonArray jsonSideboard = jsonDeck["sideboard"].toArray();
    QMap<Card*, int> sideboard = v3JsonArray2List(jsonSideboard);
    int deckTileId = jsonDeck["deckTileId"].toInt();
    QList<QPair<int, QString>> cardSkins;
    QJsonArray jsonCardSkins = jsonDeck["cardSkins"].toArray();
    for(QJsonValueRef jsonCardSkinRef : jsonCardSkins){
        QJsonObject cardSkin = jsonCardSkinRef.toObject();
        int grpId = cardSkin["grpId"].toInt();
        QString ccv = cardSkin["ccv"].toString();
        cardSkins << qMakePair(grpId, ccv);
    }
    return Deck(id, name, cards, sideboard, deckTileId, cardSkins);
}

QMap<Card*, int> MtgaLogParser::v3JsonArray2List(QJsonArray cardsArrayV3)
{
    QMap<Card*, int> cards;
    int cardId = 0;
    for(QJsonValueRef jsonCardRef : cardsArrayV3){
        int value = jsonCardRef.toInt();
        if (cardId == 0) {
            cardId = value;
        } else {
            Card* card = mtgCards->findCard(cardId);
            int qtd = value;
            if (card && qtd > 0) {
                cards[card] = qtd;
            }
            cardId = 0;
        }
    }
    return cards;
}

void MtgaLogParser::parse(QString logNewContent)
{
    if (logNewContent.contains("Event.MatchCreated")) {
        matchRunning = true;
    }
    if (matchRunning) {
        lastMatchLogMsgs.push(logNewContent);
    }
    // Extract raw msgs
    QRegularExpressionMatchIterator iterator = reRawMsg.globalMatch(logNewContent);
    QList<QString> rawMsgs;
    while (iterator.hasNext()) {
        rawMsgs << iterator.next().captured(0);
    }
    // Extract msg id and json
    // List of msgs in format (msgId, msgJson)
    QList<QPair<QString, QJsonObject>> incomingMsgs;
    QList<QPair<QString, QString>> outcomingMsgs;
    QList<QPair<QString, QString>> matchMsgs;
    for (QString msg: rawMsgs) {
        QString msgIdentifier = "";
        QRegularExpressionMatch identifierMatch = reMsgId.match(msg);
        if (identifierMatch.hasMatch()) {
            msgIdentifier = identifierMatch.captured(0);
            if (msgIdentifier.contains("(")) {
                msgIdentifier = msgIdentifier.left(msgIdentifier.indexOf("("));
            }
        }
        QString msgJson = "";
        QRegularExpressionMatch jsonMatch = reMsgJson.match(msg);
        if (jsonMatch.hasMatch()) {
            msgJson = jsonMatch.captured(0);
            if (msgJson.at(0) == '[' && msgJson.right(1) != ']') {
                msgJson += "]";
            }
        } else if (msg.contains("[Message summarized")) {
            emit sgnSummarizedMessage();
            continue;
        }

        QJsonObject json = Transformations::stringToJsonObject(msgJson);
        if (json.empty()) {
            LOGD(QString("Empty msg for: %1").arg(msgIdentifier));
            continue;
        }
        // Avoid process duplicated responses
        if (json.keys().contains("id")) {
            int msgId = json["id"].toInt();
            if (msg.contains("==>")) {
                if (msgRequestIds.contains(msgId)) {
                    LOGD("Duplicated request msg")
                    continue;
                } else {
                    msgRequestIds << msgId;
                    // Store only last 4 msg response number
                    if (msgRequestIds.size() >= 5) {
                        msgRequestIds.removeFirst();
                    }
                }
            } else {
                if (msgResponseIds.contains(msgId)) {
                    LOGD("Duplicated response msg")
                    continue;
                } else {
                    msgResponseIds << msgId;
                    // Store only last 4 msg response number
                    if (msgResponseIds.size() >= 5) {
                        msgResponseIds.removeFirst();
                    }
                }
            }
        }
        if (json.keys().contains("request")) {
            QString body = json["request"].toString();
            outcomingMsgs << QPair<QString, QString>(msgIdentifier, body);
        } else if (json.keys().contains("payload")) {
            QJsonObject body = json["payload"].toObject();
            incomingMsgs << QPair<QString, QJsonObject>(msgIdentifier, body);
        } else if (msg.contains(" to Match:") || msg.contains(": Match to ")) {
            matchMsgs << QPair<QString, QString>(msgIdentifier, msgJson);
        }
    }
    // Log msgs
    for (QPair<QString, QString> msg: outcomingMsgs) {
        parseOutcomingMsg(msg);
    }
    for (QPair<QString, QJsonObject> msg: incomingMsgs) {
        parseIncomingMsg(msg);
    }
    for (QPair<QString, QString> msg: matchMsgs) {
        parseMatchMsg(msg);
    }
}

QStack<QString> MtgaLogParser::getLastMatchLog()
{
    matchRunning = false;
    QStack<QString> matchLogMsgs;
    while (!lastMatchLogMsgs.isEmpty()) {
        matchLogMsgs.push(lastMatchLogMsgs.pop());
    }
    return matchLogMsgs;
}

void MtgaLogParser::parseOutcomingMsg(QPair<QString, QString> msg)
{
#ifdef QT_DEBUG
    if (msg.first != "Log.BI") {
        LOGD(QString("Outcoming msg: %1 - %2").arg(msg.first).arg(msg.second));
    }
#endif
    if (msg.first == "Authenticate") {
        parseAuthenticate(msg.second);
    } else if (msg.first == "DirectGame.Challenge" || msg.first == "Event.AIPractice") {
        parseAIPracticeOrDirectGameDeck(msg.second);
    } else if (msg.first == "Log.BI") {
        parseLogInfo(msg.second);
    } else if (msg.first == "Draft.MakePick") {
        parseDraftPick(msg.second);
    }
}

void MtgaLogParser::parseIncomingMsg(QPair<QString, QJsonObject> msg)
{
#ifdef QT_DEBUG
    if (msg.first != "Log.BI") {
        QString json(QJsonDocument(msg.second).toJson());
        LOGD(QString("Incoming msg: %1 - %2").arg(msg.first).arg(json));
    }
#endif
    if (msg.first == "PlayerInventory.GetPlayerInventory") {
        parsePlayerInventory(msg.second);
    } else if (msg.first == "Inventory.Updated"){
        parsePlayerInventoryUpdate(msg.second);
    } else if (msg.first == "PlayerInventory.GetPlayerCardsV3"){
        parsePlayerCollection(msg.second);
    } else if (msg.first == "Deck.GetDeckListsV3"){
        parsePlayerDecks(msg.second);
    } else if (msg.first == "Deck.CreateDeckV3"){
        parsePlayerDeckCreate(msg.second);
    } else if (msg.first == "Deck.UpdateDeckV3"){
        parsePlayerDeckUpdate(msg.second);
    } else if (msg.first == "Event.GetPlayerCourseV2"){
        parseEventPlayerCourse(msg.second);
    } else if (msg.first == "Event.GetPlayerCourses"){
        parseEventPlayerCourses(msg.second);
    } else if (msg.first == "Event.MatchCreated"){
        parseMatchCreated(msg.second);
    } else if (msg.first == "Event.GetCombinedRankInfo"){
        parsePlayerRankInfo(msg.second);
    } else if (msg.first == "Rank.Updated"){
        parsePlayerRankUpdated(msg.second);
    } else if (msg.first == "MythicRating.Updated"){
        parsePlayerMythicRatingUpdated(msg.second);
    } else if (msg.first == "Event.DeckSubmitV3"){
        parsePlayerDeckSubmited(msg.second);
    } else if (msg.first == "Event.ClaimPrize") {
        parseEventFinish(msg.second);
    } else if (msg.first == "Draft.DraftStatus" || msg.first == "Draft.MakePick") {
        parseDraftStatus(msg.second);
    }
}

void MtgaLogParser::parseMatchMsg(QPair<QString, QString> msg)
{
#ifdef QT_DEBUG
    if (msg.first != "GreToClientEvent") {
        LOGD(QString("Match msg: %1 - %2").arg(msg.first).arg(msg.second));
    }
#endif

    if (msg.first == "ClientToMatchServiceMessageType_ClientToGREMessage") {
        parseClientToGreMessages(msg.second);
    } else if (msg.first == "MatchGameRoomStateChangedEvent"){
        parseMatchInfo(msg.second);
    } else if (msg.first == "GreToClientEvent"){
        parseGreToClientMessages(msg.second);
    }
}

void MtgaLogParser::parsePlayerInventory(QJsonObject json)
{
    int gold = json["gold"].toInt();
    int gems = json["gems"].toInt();
    int wcCommon = json["wcCommon"].toInt();
    int wcUncommon = json["wcUncommon"].toInt();
    int wcRare = json["wcRare"].toInt();
    int wcMythic = json["wcMythic"].toInt();
    double vaultProgress = json["vaultProgress"].toDouble();
    PlayerInventory playerInventory(gold, gems, wcCommon, wcUncommon, wcRare, wcMythic, vaultProgress);
    LOGD(QString("PlayerInventory: %1 wcC, %2 wcI, %3 wcR, %4 wcM")
         .arg(wcCommon).arg(wcUncommon).arg(wcRare).arg(wcMythic));
    emit sgnPlayerInventory(playerInventory);
}

void MtgaLogParser::parsePlayerInventoryUpdate(QJsonObject json)
{
    QJsonObject delta = json["delta"].toObject();
    QJsonArray jsonCards = delta["cardsAdded"].toArray();

    QList<int> newCards;
    for (QJsonValueRef jsonCardRef: jsonCards) {
        int newCardId = jsonCardRef.toInt();
        newCards << newCardId;
    }
    LOGD(QString("PlayerInventoryUpdate: %1 new cards").arg(newCards.size()));
    emit sgnPlayerInventoryUpdate(newCards);
}

void MtgaLogParser::parsePlayerCollection(QJsonObject json)
{
    QMap<int, int> ownedCards;
    for (QString ownedCardId: json.keys()) {
        int ownedCardQtd = json[ownedCardId].toInt();
        ownedCards[ownedCardId.toInt()] = ownedCardQtd;
    }
    LOGD(QString("PlayerCollection: %1 unique cards").arg(ownedCards.size()));
    emit sgnPlayerCollection(ownedCards);
}

void MtgaLogParser::parsePlayerDecks(QJsonObject json)
{
    QList<Deck> playerDecks;
    for (QJsonValueRef jsonDeckRef: json) {
        QJsonObject jsonDeck = jsonDeckRef.toObject();
        playerDecks << jsonObject2DeckV3(jsonDeck);
    }
    LOGD(QString("PlayerDecks: %1 decks").arg(playerDecks.size()));
    emit sgnPlayerDecks(playerDecks);
}

void MtgaLogParser::parseEventPlayerCourse(QJsonObject json)
{
    QString eventId = json["InternalEventName"].toString();
    if (!json["CourseDeck"].isNull()) {
        QString currentModule = json["CurrentModule"].toString();
        QJsonObject jsonEventPlayerCourseDeck = json["CourseDeck"].toObject();
        Deck deck = jsonObject2DeckV3(jsonEventPlayerCourseDeck);
        bool isFinished = currentModule == "ClaimPrize";
        LOGD(QString("EventPlayerCourse: %1 with %2. Finished: %3").arg(eventId).arg(deck.name).arg(isFinished));
        QJsonObject jsonWinLossGate = json["ModuleInstanceData"].toObject()["WinLossGate"].toObject();
        int maxWins = jsonWinLossGate["MaxWins"].toInt(-1);
        int maxLosses = jsonWinLossGate["MaxLosses"].toInt(-1);
        int currentWins = jsonWinLossGate["CurrentWins"].toInt(-1);
        int currentLosses = jsonWinLossGate["CurrentLosses"].toInt(-1);
        QJsonArray processedMatchIds = jsonWinLossGate["ProcessedMatchIds"].toArray();
        EventPlayerCourse eventPlayerCourse(eventId, deck, maxWins, maxLosses,
                                            currentWins, currentLosses, processedMatchIds);
        emit sgnEventPlayerCourse(eventPlayerCourse, isFinished);
    }
}

void MtgaLogParser::parseEventPlayerCourses(QJsonObject json)
{
    QList<QString> events;
    for (QJsonValueRef jsonEventRef: json) {
        QJsonObject event = jsonEventRef.toObject();
        events << event["InternalEventName"].toString();
    }
    LOGD(QString("EventPlayerCourses: %1 events").arg(events.size()));
    emit sgnEventPlayerCourses(events);
}

void MtgaLogParser::parseMatchCreated(QJsonObject json)
{
    QString opponentName = json["opponentScreenName"].toString();
    QString opponentRankClass = json["opponentRankingClass"].toString();
    int opponentRankTier = json["opponentRankingTier"].toInt();
    int opponentMythicLeaderboardPlace = json["opponentMythicLeaderboardPlace"].toInt();
    double opponentMythicPercentile = json["opponentMythicPercentile"].toDouble();
    QJsonArray opponentCommanderGrpIds = json["opponentCommanderGrpIds"].toArray();
    QMap<Card*, int> opponentCommanders = v3JsonArray2List(opponentCommanderGrpIds);
    QJsonArray commanderGrpIds = json["commanderGrpIds"].toArray();
    QMap<Card*, int> playerCommanders = v3JsonArray2List(commanderGrpIds);
    QString matchId = json["matchId"].toString();
    QString eventId = json["eventId"].toString();
    RankInfo opponentInfo(opponentRankClass, opponentRankTier, -1,
                          opponentMythicLeaderboardPlace, opponentMythicPercentile);
    LOGD(QString("MatchCreated: Opponent %1, rank: %2(%3)").arg(opponentName)
         .arg(opponentRankClass).arg(opponentRankTier));
    emit sgnMatchCreated(matchId, eventId, playerCommanders,
                         opponentName, opponentInfo, opponentCommanders);
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
            QString playerAccountId = jsonPlayer["userId"].toString();
            int playerSeat = jsonPlayer["systemSeatId"].toInt();
            int playerTeamId = jsonPlayer["teamId"].toInt();
            matchPlayers << MatchPlayer(playerName, playerAccountId, playerSeat, playerTeamId);
        }
        LOGD("MatchInfoSeats");
        emit sgnMatchInfoSeats(matchPlayers);
    }
    if (roomState == "MatchGameRoomStateType_MatchCompleted"){
        QJsonObject jsonFinalMatchResult = jsonRoomInfo["finalMatchResult"].toObject();
        QJsonArray jsonResults = jsonFinalMatchResult["resultList"].toArray();
        for (QJsonValueRef jsonPlayerRef : jsonResults) {
            QJsonObject jsonResult = jsonPlayerRef.toObject();
            QString scope = jsonResult["scope"].toString();
            if (scope == "MatchScope_Match") {
                QString result = jsonResult["result"].toString();
                int winningTeamId = jsonResult["winningTeamId"].toInt();
                emit sgnMatchResult(ResultSpec(scope, result, winningTeamId));
                LOGD("MatchInfoResult");
            }
        }
    }
}

void MtgaLogParser::parsePlayerRankInfo(QJsonObject json)
{
    QString rankClass = json["constructedClass"].toString();
    int rankTier = json["constructedLevel"].toInt();
    LOGD(QString("PlayerRankInfo: %1 - %2").arg(rankClass).arg(rankTier));
    emit sgnPlayerRankInfo(qMakePair(rankClass, rankTier));
}

void MtgaLogParser::parsePlayerRankUpdated(QJsonObject json)
{
    QString rankClass = json["newClass"].toString();
    int rankTier = json["newLevel"].toInt();
    int rankStep = json["newStep"].toInt();
    QString oldClass = json["oldClass"].toString();
    int oldTier = json["oldLevel"].toInt();
    int oldStep = json["oldStep"].toInt();
    RankInfo playerCurrentRankInfo(rankClass, rankTier, rankStep);
    RankInfo playerOldRankInfo(oldClass, oldTier, oldStep);
    int seasonOrdinal = json["seasonOrdinal"].toInt();
    emit sgnPlayerRankUpdated(playerCurrentRankInfo, playerOldRankInfo, seasonOrdinal);
}

void MtgaLogParser::parsePlayerMythicRatingUpdated(QJsonObject json)
{
    double oldMythicPercentile = json["oldMythicPercentile"].toDouble();
    double newMythicPercentile = json["newMythicPercentile"].toDouble();
    int newMythicLeaderboardPlacement = json["newMythicLeaderboardPlacement"].toInt();
    emit sgnPlayerMythicRatingUpdated(oldMythicPercentile, newMythicPercentile, newMythicLeaderboardPlacement);
}

void MtgaLogParser::parsePlayerDeckCreate(QJsonObject json)
{
    Deck deck = jsonObject2DeckV3(json);
    LOGD(QString("PlayerCreateDeck: %1").arg(deck.name));
    emit sgnPlayerDeckCreated(deck);
}

void MtgaLogParser::parsePlayerDeckUpdate(QJsonObject json)
{
    Deck deck = jsonObject2DeckV3(json);
    LOGD(QString("PlayerUpdateDeck: %1").arg(deck.name));
    emit sgnPlayerDeckUpdated(deck);
}

void MtgaLogParser::parsePlayerDeckSubmited(QJsonObject json)
{
    QString eventId = json["InternalEventName"].toString();
    QJsonObject jsonDeck = json["CourseDeck"].toObject();
    Deck deckSubmited = jsonObject2DeckV3(jsonDeck);
    LOGD(QString("Deck submited: %1").arg(deckSubmited.name));
    emit sgnPlayerDeckSubmited(eventId, deckSubmited);
}

void MtgaLogParser::parseAIPracticeOrDirectGameDeck(QString json)
{
    QJsonObject jsonMessage = Transformations::stringToJsonObject(json);
    QJsonObject jsonParams = jsonMessage["params"].toObject();
    QString jsonDeckString = jsonParams["deck"].toString().replace("\\", "")
            .replace("\"Id\"", "\"id\"").replace("\"Quantity\"", "\"quantity\"");
    QJsonObject jsonDeck = Transformations::stringToJsonObject(jsonDeckString);    
    Deck deckSubmited;
    if (jsonDeckString.contains("quantity")) {
        deckSubmited = jsonObject2DeckV1(jsonDeck);
    } else {
        deckSubmited = jsonObject2DeckV3(jsonDeck);
    }
    LOGD(QString("Deck submited: %1").arg(deckSubmited.name));
    QString eventName = jsonParams.contains("botDeckId") ? "AIPractice" : "DirectGame";
    emit sgnPlayerDeckSubmited(eventName, deckSubmited);
}

void MtgaLogParser::parseGreToClientMessages(QString json)
{
    if (json.contains("[Message summarized")) {
        emit sgnSummarizedMessage();
        return;
    }
    QJsonObject jsonGreToClientMsg = Transformations::stringToJsonObject(json);
    if (jsonGreToClientMsg.empty()) {
        return;
    }
    QJsonObject jsonGreToClientEvent = jsonGreToClientMsg["greToClientEvent"].toObject();
    QJsonArray jsonGreToClientMessages = jsonGreToClientEvent["greToClientMessages"].toArray();
    for (QJsonValueRef jsonMessageRef : jsonGreToClientMessages) {
        QJsonObject jsonMessage = jsonMessageRef.toObject();
        QString messageType = jsonMessage["type"].toString();
        if (messageType == "GREMessageType_GameStateMessage" ||
                   messageType == "GREMessageType_QueuedGameStateMessage") {
            int gameStateId = jsonMessage["gameStateId"].toInt();
            QJsonObject jsonGameStateMessage = jsonMessage["gameStateMessage"].toObject();
            if (jsonGameStateMessage.contains("turnInfo")) {
                QJsonObject jsonTurnInfo = jsonGameStateMessage["turnInfo"].toObject();
                if (jsonTurnInfo.contains("activePlayer")) {
                    int activePlayer = jsonTurnInfo["activePlayer"].toInt();
                    emit sgnActivePlayer(activePlayer);
                }
                if (jsonTurnInfo.contains("decisionPlayer")) {
                    int decisionPlayer = jsonTurnInfo["decisionPlayer"].toInt();
                    emit sgnDecisionPlayer(decisionPlayer);
                }
            }
            QString gameStateType = jsonGameStateMessage["type"].toString();
            if (gameStateType == "GameStateType_Full") {
                parseGameStateFull(jsonGameStateMessage);
            } else if (gameStateType == "GameStateType_Diff") {
                QJsonArray systemSeatIds = jsonMessage["systemSeatIds"].toArray();
                int playerSeatId = systemSeatIds.size() > 0 ? systemSeatIds.first().toInt() : 0;
                parseGameStateDiff(playerSeatId, gameStateId, jsonGameStateMessage);
            }
        }
    }
}

void MtgaLogParser::parseGameStateFull(QJsonObject jsonMessage)
{
    QJsonObject gameInfoJson = jsonMessage["gameInfo"].toObject();
    QString mulliganType = gameInfoJson["mulliganType"].toString();
    int number = gameInfoJson["gameNumber"].toInt();
    QString superFormat = gameInfoJson["superFormat"].toString();
    QString type = gameInfoJson["type"].toString();
    QString variant = gameInfoJson["variant"].toString();
    QString winCondition = gameInfoJson["matchWinCondition"].toString();
    GameInfo gameInfo(mulliganType, number, superFormat, type, variant, winCondition);
    QList<MatchZone> zones = getMatchZones(jsonMessage);
    int seatId = jsonMessage["turnInfo"].toObject()["decisionPlayer"].toInt();
    LOGD(QString("GameStart WinCondition: %1, Zones: %2, DecisionPlayer: %3")
         .arg(winCondition).arg(zones.size()).arg(seatId));
    emit sgnGameStart(gameInfo, zones, seatId);
}

void MtgaLogParser::parseGameStateDiff(int playerSeatId, int gameStateId, QJsonObject jsonMessage)
{
    QJsonObject jsonInfoObject = jsonMessage["gameInfo"].toObject();
    if (jsonInfoObject["matchState"].toString() == "MatchState_GameComplete") {
        QJsonArray jsonResults = jsonInfoObject["results"].toArray();
        QJsonObject jsonResult = jsonResults.last().toObject();
        QString scope = jsonResult["scope"].toString();
        QString reason = jsonResult["reason"].toString();
        QString result = jsonResult["result"].toString();
        int winningTeamId = jsonResult["winningTeamId"].toInt();
        emit sgnGameCompleted(ResultSpec(scope, result, winningTeamId, reason));
        return;
    }
    QList<MatchZone> zones = getMatchZones(jsonMessage);
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
    bool hasShuffleAnnotation = false;
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
        if (type == "AnnotationType_Shuffle") {
            hasShuffleAnnotation = true;
        }
    }
    QJsonArray jsonDiffDeletedInstanceIds = jsonMessage["diffDeletedInstanceIds"].toArray();
    if (jsonDiffDeletedInstanceIds.size() >= 40 && !hasShuffleAnnotation) {
        QList<int> diffDeletedInstanceIds;
        for (QJsonValueRef jsonDiffDeletedInstanceIdsRef : jsonDiffDeletedInstanceIds) {
            diffDeletedInstanceIds << jsonDiffDeletedInstanceIdsRef.toInt();
        }
        checkMulligans(playerSeatId, diffDeletedInstanceIds, zones);
    }
    QMap<int, int> idsChanged = getIdsChanged(jsonGSMAnnotations);
    QList<QPair<int, int>> revealedCardCreated = getIdsRevealedCardCreated(jsonGSMAnnotations, jsonGameObjects);
    QMap<int, MatchZoneTransfer> idsZoneChanged = getIdsZoneChanged(jsonGSMAnnotations);
    MatchStateDiff matchStateDiff(gameStateId, zones, idsChanged, idsZoneChanged, revealedCardCreated);
    emit sgnMatchStateDiff(matchStateDiff);
    return;
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

void MtgaLogParser::checkMulligans(int playerSeatId, QList<int> diffDeletedInstanceIds,
                                   QList<MatchZone> zones)
{
    for (MatchZone zone : zones) {
        if (zone.type() == ZoneType_LIBRARY) {
            QList<int> zoneIds = zone.objectIds.keys();
            if (listContainsSublist(diffDeletedInstanceIds, zoneIds)) {
                int seatId = zone.ownerSeatId();
                if (seatId == playerSeatId) {
                    QMap<int, int> newHandDrawed;
                    for (MatchZone zone2 : zones) {
                        if (zone2.type() == ZoneType_HAND && zone2.ownerSeatId() == seatId) {
                            newHandDrawed = zone2.objectIds;
                            break;
                        }
                    }
                    LOGD("Player mulligan");
                    emit sgnPlayerTakesMulligan(newHandDrawed);
                } else {
                    LOGD("OpponentTakesMulligan");
                    emit sgnOpponentTakesMulligan(seatId);
                }
            }
        }
    }
}

bool MtgaLogParser::listContainsSublist(QList<int> list, QList<int> subList)
{
    for (int item : subList) {
        if (!list.contains(item)) {
            return false;
        }
    }
    return true;
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
            int srcZone = 0;
            int dstZone = 0;
            ZoneTransferCategory transferCategory = ZoneTransfer_UNKOWN;
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

QList<QPair<int, int>> MtgaLogParser::getIdsRevealedCardCreated(QJsonArray jsonGSMAnnotations, QJsonArray jsonGameObjects)
{
    QList<QPair<int, int>> revealedCardCreated;
    for (QJsonValueRef jsonAnnotationRef : jsonGSMAnnotations) {
        QJsonObject jsonAnnotation = jsonAnnotationRef.toObject();
        QString type = jsonAnnotation["type"].toArray().first().toString();
        if (type == "AnnotationType_RevealedCardCreated") {
            int instanceId = jsonAnnotation["affectedIds"].toArray().first().toInt();
            for (QJsonValueRef jsonGameObjectRef : jsonGameObjects) {
                QJsonObject jsonGameObject = jsonGameObjectRef.toObject();
                if(jsonGameObject["instanceId"].toInt() == instanceId) {
                    int grpId = jsonGameObject["grpId"].toInt();
                    int ownerId = jsonGameObject["ownerSeatId"].toInt();
                    revealedCardCreated << qMakePair(grpId, ownerId);
                    break;
                }
            }
        }
    }
    return revealedCardCreated;
}

void MtgaLogParser::parseClientToGreMessages(QString json)
{
    QJsonObject jsonClientToGreMsg = Transformations::stringToJsonObject(json);
    if (jsonClientToGreMsg.empty()) {
        return;
    }
    QString type = jsonClientToGreMsg["clientToMatchServiceMessageType"].toString();
    QString payload = jsonClientToGreMsg["payload"].toString();
    if (type == "ClientToMatchServiceMessageType_ClientToGREMessage" && payload.size() >= 200) {
        emit sgnDecodeDeckPosSideboardPayload(type, payload);
    }
}

void MtgaLogParser::onParseDeckPosSideboardJson(QJsonObject jsonMessage)
{
    if (!jsonMessage.contains("submitdeckresp")) {
        return;
    }
    QJsonObject jsonDeckResp = jsonMessage["submitdeckresp"].toObject();
    QJsonObject jsonDeck = jsonDeckResp["deck"].toObject();
    QJsonArray jsonMainDeckCards = jsonDeck["deckcardsList"].toArray();
    QMap<Card*, int> mainDeck;
    for(QJsonValueRef jsonCardRef : jsonMainDeckCards){
        int cardId = jsonCardRef.toInt();
        Card* card = mtgCards->findCard(cardId);
        if (mainDeck.keys().contains(card)) {
            mainDeck[card] += 1;
        } else {
            mainDeck[card] = 1;
        }
    }
    QJsonArray jsonSideboardCards = jsonDeck["sideboardcardsList"].toArray();
    QMap<Card*, int> sideboard;
    for(QJsonValueRef jsonCardRef : jsonSideboardCards){
        int cardId = jsonCardRef.toInt();
        Card* card = mtgCards->findCard(cardId);
        if (sideboard.keys().contains(card)) {
            sideboard[card] += 1;
        } else {
            sideboard[card] = 1;
        }
    }
    emit sgnPlayerDeckWithSideboardSubmited(mainDeck, sideboard);
}

void MtgaLogParser::parseEventFinish(QJsonObject json)
{
    QString eventState = json["CurrentModule"].toString();
    if (eventState != "Complete") {
        return;
    }    
    QString eventId = json["InternalEventName"].toString();
    QJsonObject jsonCourseDeck = json["CourseDeck"].toObject();
    Deck deck = jsonObject2DeckV3(jsonCourseDeck);
    QJsonObject jsonWinLossGate = json["ModuleInstanceData"].toObject()["WinLossGate"].toObject();
    int maxWins = jsonWinLossGate["MaxWins"].toInt();
    int wins = jsonWinLossGate["CurrentWins"].toInt();
    int losses = jsonWinLossGate["CurrentLosses"].toInt();
    emit sgnEventFinish(eventId, deck.id, deck.colorIdentity(), maxWins, wins, losses);
}

void MtgaLogParser::parseAuthenticate(QString json)
{
    QJsonObject jsonAuthenticate = Transformations::stringToJsonObject(json);
    if (jsonAuthenticate.empty()) {
        return;
    }
    QJsonObject params = jsonAuthenticate["params"].toObject();
    QString version = params["clientVersion"].toString();
    emit sgnMtgaClientVersion(version);
}

void MtgaLogParser::parseDraftPick(QString json)
{
    QJsonObject jsonLogInfo = Transformations::stringToJsonObject(json);
    if (jsonLogInfo.empty()) {
        return;
    }
    QJsonObject params = jsonLogInfo["params"].toObject();
    int cardId = params["cardId"].toString().toInt();
    int packNumber = params["packNumber"].toString().toInt();
    int pickNumber = params["pickNumber"].toString().toInt();
    emit sgnDraftPick(cardId, packNumber, pickNumber);
}

void MtgaLogParser::parseDraftStatus(QJsonObject json)
{
    QString eventId = json["DraftId"].toString().split(":")[1];
    QString status = json["DraftStatus"].toString();
    int packNumber = json["PackNumber"].toInt();
    int pickNumber = json["PickNumber"].toInt();
    QList<Card*> availablePicks;
    QJsonArray jsonAvailablePicks = json["DraftPack"].toArray();
    for(QJsonValueRef jsonAvailablePickRef : jsonAvailablePicks){
        int mtgaId = jsonAvailablePickRef.toString().toInt();
        availablePicks << LOTUS_TRACKER->mtgCards->findCard(mtgaId);
    }
    QList<Card*> pickedCards;
    QJsonArray jsonPickedCards = json["PickedCards"].toArray();
    for(QJsonValueRef jsonPickedCardRef : jsonPickedCards){
        int mtgaId = jsonPickedCardRef.toString().toInt();
        pickedCards << LOTUS_TRACKER->mtgCards->findCard(mtgaId);
    }
    emit sgnDraftStatus(eventId, status, packNumber, pickNumber, availablePicks, pickedCards);
}

void MtgaLogParser::parseLogInfo(QString json)
{
    QJsonObject jsonLogInfo = Transformations::stringToJsonObject(json);
    if (jsonLogInfo.empty()) {
        return;
    }
    try {
        QJsonObject params = jsonLogInfo["params"].toObject();
        QString messageName = params["messageName"].toString();
        if (messageName == "Client.SceneChange") {
            QJsonObject payloadObject = params["payloadObject"].toObject();
            QString fromSceneName = payloadObject["fromSceneName"].toString();
            QString toSceneName = payloadObject["toSceneName"].toString();
            if (fromSceneName == "Draft") {
                emit sgnLeavingDraft();
            }
            if (toSceneName == "Home") {
                emit sgnGoingToHome();
            }
        }
    } catch (const std::exception& ex) {
        LOGW(ex.what())
    }

}
