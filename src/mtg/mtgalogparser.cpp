#include "mtgalogparser.h"
#include "../extensions.h"
#include "../macros.h"

#include <QList>
#include <QRegularExpression>

#define REGEXP_RAW_MSG "\\s(Response|Incoming|Match\\sto|to\\sMatch).+(\\s|\\n)\\{(\\n\\s+.*)+\\n\\}"
#define REGEXP_MSG_RESPONSE_NUMBER "(?<=\\s)\\d+(?=\\:\\s)"
#define REGEXP_MSG_ID "\\S+(?=(\\s|\\n)\\{)"
#define REGEXP_MSG_JSON "\\{(\\n\\s+.*)+\\n\\}"

MtgaLogParser::MtgaLogParser(QObject *parent) : QObject(parent)
{

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
        parseOpponentInfo(msg.second);
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
    QString playerId = jsonPlayerIventory["playerId"].toString();
    int wcCommon = jsonPlayerIventory["wcCommon"].toInt();
    int wcUncommon = jsonPlayerIventory["wcUncommon"].toInt();
    int wcRare = jsonPlayerIventory["wcRare"].toInt();
    int wcMythic = jsonPlayerIventory["wcMythic"].toInt();
    float vaultProgress = jsonPlayerIventory["vaultProgress"].toDouble();
    PlayerInventory playerInventory(playerId, wcCommon, wcUncommon, wcRare, wcMythic, vaultProgress);
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

}

void MtgaLogParser::parsePlayerDecks(QString json)
{

}

void MtgaLogParser::parseOpponentInfo(QString json)
{

}

void MtgaLogParser::parseMatchInfo(QString json)
{

}

void MtgaLogParser::parsePlayerRankInfo(QString json)
{

}

void MtgaLogParser::parsePlayerDeckSelected(QString json)
{

}

void MtgaLogParser::parsePlayerMulliganInfo(QString json)
{

}

void MtgaLogParser::parsePlayerMatchState(QString json)
{

}
