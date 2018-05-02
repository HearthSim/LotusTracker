#include "mtgalogparser.h"
#include "../arenatracker.h"
#include "../extensions.h"
#include "../macros.h"

#include <QList>
#include <QRegularExpression>

#define REGEXP_RAW_MSG "\\s(Response|Incoming|Match\\sto|to\\sMatch).+(\\s|\\n)(\\{|\\[)(\\n\\s+.*)+\\n(\\}|\\])"
#define REGEXP_MSG_RESPONSE_NUMBER "(?<=\\s)\\d+(?=\\:\\s)"
#define REGEXP_MSG_ID "\\S+(?=(\\s|\\n)(\\{|\\[))"
#define REGEXP_MSG_JSON "(\\{|\\[)(\\n\\s+.*)+\\n(\\}||\\])"

MtgaLogParser::MtgaLogParser(QObject *parent, MtgCards* mtgCards)
    : QObject(parent), mtgCards(mtgCards)
{
    if (mtgCards == NULL) {
        mtgCards = new MtgCards(this);
    }
}

MtgaLogParser::~MtgaLogParser()
{
    DELETE(mtgCards);
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
    LOGD(QString("Receive msg %1 with json size: %2").arg(msg.first).arg(msg.second.size()));
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
    } else if (msg.first == "Event.DeckSelect"){
        parsePlayerDeckSelected(msg.second);
    } else if (msg.first == "ClientToGreMessage"){
        parsePlayerMulliganInfo(msg.second);
    } else if (msg.first == "GreToClientEvent"){
        parsePlayerMatchState(msg.second);
    }
}

void MtgaLogParser::parsePlayerInventory(QString json)
{
    QJsonObject jsonPlayerIventory = Extensions::stringToJsonObject(json);
    if (jsonPlayerIventory.empty()) {
        return;
    }
    int wcCommon = jsonPlayerIventory["wcCommon"].toInt();
    int wcUncommon = jsonPlayerIventory["wcUncommon"].toInt();
    int wcRare = jsonPlayerIventory["wcRare"].toInt();
    int wcMythic = jsonPlayerIventory["wcMythic"].toInt();
    float vaultProgress = jsonPlayerIventory["vaultProgress"].toDouble();
    PlayerInventory playerInventory(wcCommon, wcUncommon, wcRare, wcMythic, vaultProgress);
    emit sgnPlayerInventory(playerInventory);
}

void MtgaLogParser::parsePlayerInventoryUpdate(QString json)
{
    QJsonObject jsonPlayerIventoryUpdate = Extensions::stringToJsonObject(json);
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
    emit sgnPlayerInventoryUpdate(newCards);
}

void MtgaLogParser::parsePlayerCollection(QString json)
{
    QJsonObject jsonPlayerCollection = Extensions::stringToJsonObject(json);
    if (jsonPlayerCollection.empty()) {
        return;
    }
    QMap<int, int> ownedCards;
    for (QString ownedCardId: jsonPlayerCollection.keys()) {
        int ownedCardQtd = jsonPlayerCollection[ownedCardId].toInt();
        ownedCards[ownedCardId.toInt()] = ownedCardQtd;
    }
    emit sgnPlayerCollection(ownedCards);
}

void MtgaLogParser::parsePlayerDecks(QString json)
{
    QJsonArray jsonPlayerDecks = Extensions::stringToJsonArray(json);
    if (jsonPlayerDecks.empty()) {
        return;
    }
    QList<Deck> playerDecks;
    for (QJsonValueRef jsonDeckRef: jsonPlayerDecks) {
        QJsonObject jsonDeck = jsonDeckRef.toObject();
        playerDecks << jsonObject2Deck(jsonDeck);
    }
    emit sgnPlayerDecks(playerDecks);
}

void MtgaLogParser::parseMatchCreated(QString json)
{
    QJsonObject jsonMatchCreated = Extensions::stringToJsonObject(json);
    if (jsonMatchCreated.empty()) {
        return;
    }
    QString opponentName = jsonMatchCreated["opponentScreenName"].toString();
    QString opponentRankClass = jsonMatchCreated["opponentRankingClass"].toString();
    int opponentRankTier = jsonMatchCreated["opponentRankingTier"].toInt();
    Match match(opponentName, opponentRankClass, opponentRankTier);
    emit sgnMatchCreated(match);
}

void MtgaLogParser::parseMatchInfo(QString json)
{
    QJsonObject jsonMatchInfo = Extensions::stringToJsonObject(json);
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
        emit sgnMatchInfoResultMatch(matchWinningTeamId);
    }
}

void MtgaLogParser::parsePlayerRankInfo(QString json)
{

}

void MtgaLogParser::parsePlayerDeckSelected(QString json)
{
    QJsonObject jsonPlayerDeckSelected = Extensions::stringToJsonObject(json);
    if (jsonPlayerDeckSelected.empty()) {
        return;
    }
    QJsonObject jsonDeck = jsonPlayerDeckSelected["CourseDeck"].toObject();
    Deck deckSelected = jsonObject2Deck(jsonDeck);
    emit sgnPlayerDeckSelected(deckSelected);
}

void MtgaLogParser::parsePlayerMulliganInfo(QString json)
{

}

void MtgaLogParser::parsePlayerMatchState(QString json)
{

}
