#include "untappedmatchdescriptor.h"
#include "../macros.h"

UntappedMatchDescriptor::UntappedMatchDescriptor(QObject *parent) : QObject(parent)
{

}

QJsonDocument UntappedMatchDescriptor::prepareNewDescriptor(MatchInfo matchInfo, QString timestamp,
                                                            EventPlayerCourse eventPlayerCourse)
{
    this->matchInfo = matchInfo;
    return QJsonDocument(
        QJsonObject({
            { "timestamp", timestamp },
            { "summarizedMessageCount", matchInfo.summarizedMessage },
            { "client", QString("lt-%1").arg(qApp->applicationVersion()) },
            { "mtgaVersion", LOTUS_TRACKER->mtgArena->getClientVersion() },
            { "upload_token", APP_SETTINGS->getUntappedAnonymousUploadToken() },
            { "match", QJsonObject({
                { "matchId", matchInfo.matchId },
                { "deck", deckToJsonObject(matchInfo.games[0].playerDeck) },
                { "games", getMatchGamesDescriptor() },
                { "player", getMatchPlayerDescriptor() },
                { "opponents", getMatchOpponentsDescriptor() },
                { "result", resultSpecToJsonObject(matchInfo.resultSpec) },
                { "seasonOrdinal", matchInfo.seasonOrdinal }
            })},
            { "event", getMatchEventDescriptor(eventPlayerCourse) }
        })
    );
}

QJsonArray UntappedMatchDescriptor::getMatchGamesDescriptor()
{
    QJsonArray games;
    for(GameInfo game : matchInfo.games) {
        games.append(QJsonObject({
            { "deck", deckToJsonObject(game.playerDeck) },
            { "duration", game.duration },
            { "opponentRevealedCards", cardsToJsonArray(game.opponentRevealedDeck.currentCards()) },
            { "result", resultSpecToJsonObject(game.resultSpec) }
        }));
    }
    return games;
}

QJsonObject UntappedMatchDescriptor::getMatchPlayerDescriptor()
{
    return QJsonObject({
       { "name", matchInfo.player.name() },
       { "accountId", matchInfo.player.accountId() },
       { "teamId", matchInfo.player.teamId() },
       { "systemSeatId", matchInfo.player.seatId() },
       { "preMatchRankInfo", QJsonObject({
           { "rankClass", matchInfo.playerCurrentRankInfo.rankClass() },
           { "tier", intToJsonValue(matchInfo.playerCurrentRankInfo.rankTier()) },
           { "step", intToJsonValue(matchInfo.playerCurrentRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intToJsonValue(matchInfo.playerCurrentRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleToJsonValue(matchInfo.playerCurrentRankInfo.mythicPercentile()) }
       })},
       { "postMatchRankInfo", QJsonObject({
           { "rankClass", matchInfo.playerOldRankInfo.rankClass() },
           { "tier", intToJsonValue(matchInfo.playerOldRankInfo.rankTier()) },
           { "step", intToJsonValue(matchInfo.playerOldRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intToJsonValue(matchInfo.playerOldRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleToJsonValue(matchInfo.playerOldRankInfo.mythicPercentile()) }
       })}
    });
}

QJsonArray UntappedMatchDescriptor::getMatchOpponentsDescriptor()
{
    return QJsonArray({
        QJsonObject({
            { "name", matchInfo.opponent.name() },
            { "accountId", matchInfo.opponent.accountId() },
            { "teamId", matchInfo.opponent.teamId() },
            { "systemSeatId", matchInfo.opponent.seatId() },
            { "preMatchRankInfo", QJsonObject({
                { "rankClass", matchInfo.opponentRankInfo.rankClass() },
                { "tier", intToJsonValue(matchInfo.opponentRankInfo.rankTier()) },
                { "step", intToJsonValue(matchInfo.opponentRankInfo.rankStep()) },
                { "mythicLeaderboardPlace", matchInfo.opponentRankInfo.mythicLeaderboardPlace() },
                { "mythicPercentile", matchInfo.opponentRankInfo.mythicPercentile() }
            })},
            { "postMatchRankInfo", QJsonValue::Null }
        })
    });
}

QJsonObject UntappedMatchDescriptor::getMatchEventDescriptor(EventPlayerCourse eventPlayerCourse)
{
    return QJsonObject({
       { "name", matchInfo.eventId },
       { "maxWins", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.maxWins) },
       { "maxLosses", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.maxLosses) },
       { "currentWins", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.currentWins) },
       { "currentLosses", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.currentLosses) },
       { "processedMatchIds", eventPlayerCourse.eventId == matchInfo.eventId
         ? eventPlayerCourse.processedMatchIds : QJsonArray() },
     });
}

QJsonArray UntappedMatchDescriptor::cardsToJsonArray(QMap<Card *, int> cards)
{
    QJsonArray cardsArray;
    for (Card* card : cards.keys()) {
        for(int i=0; i<cards[card]; i++) {
            cardsArray.append(card->mtgaId);
        }
    }
    return cardsArray;
}

QJsonObject UntappedMatchDescriptor::deckToJsonObject(Deck deck)
{
    QJsonArray cardSkins;
    for (QPair<int, QString> cardSkin : deck.cardSkins) {
        cardSkins.append(QJsonObject({
            { "grpId", cardSkin.first },
            { "ccv", cardSkin.second }
        }));
    }
    return QJsonObject({
       { "mainDeck", cardsToJsonArray(deck.cards()) },
       { "sideboard", cardsToJsonArray(deck.sideboard()) },
       { "name", deck.name },
       { "boxId", deck.id },
       { "deckTileId", deck.deckTileId },
       { "cardSkins", cardSkins }
    });
}

QJsonObject UntappedMatchDescriptor::resultSpecToJsonObject(ResultSpec resultSpec)
{
    QJsonObject resultJson({
        { "scope", resultSpec.scope },
        { "result", resultSpec.result },
        { "winningTeamId", resultSpec.winningTeamId }
    });
    if (!resultSpec.reason.isEmpty()) {
        resultJson.insert("reason", resultSpec.reason);
    }
    return resultJson;
}

QJsonValue UntappedMatchDescriptor::eventCourseIntToJsonValue(QString eventId, int value)
{
    if (eventId != matchInfo.eventId) {
        return QJsonValue::Null;
    }
    return intToJsonValue(value);
}

QJsonValue UntappedMatchDescriptor::intToJsonValue(int value)
{
    if (value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}

QJsonValue UntappedMatchDescriptor::doubleToJsonValue(double value)
{
    if (value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}
