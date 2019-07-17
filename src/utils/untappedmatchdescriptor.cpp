#include "untappedmatchdescriptor.h"
#include "../macros.h"

UntappedMatchDescriptor::UntappedMatchDescriptor(QObject *parent) : QObject(parent)
{

}

QJsonDocument UntappedMatchDescriptor::prepareNewDescriptor(MatchDetails matchDetails, QString timestamp,
                                                            EventPlayerCourse eventPlayerCourse)
{
    this->matchDetails = matchDetails;
    return QJsonDocument(
        QJsonObject({
            { "timestamp", timestamp },
            { "summarizedMessageCount", matchDetails.summarizedMessage },
            { "client", QString("lt-%1").arg(qApp->applicationVersion()) },
            { "mtgaVersion", LOTUS_TRACKER->mtgArena->getClientVersion() },
            { "upload_token", APP_SETTINGS->getUntappedAnonymousUploadToken() },
            { "match", QJsonObject({
                { "matchId", matchDetails.matchId },
                { "deck", deckToJsonObject(matchDetails.games[0].playerDeck) },
                { "games", getMatchGamesDescriptor() },
                { "player", getMatchPlayerDescriptor() },
                { "opponents", getMatchOpponentsDescriptor() },
                { "result", resultSpecToJsonObject(matchDetails.resultSpec) },
                { "seasonOrdinal", matchDetails.seasonOrdinal }
            })},
            { "event", getMatchEventDescriptor(eventPlayerCourse) }
        })
    );
}

QJsonArray UntappedMatchDescriptor::getMatchGamesDescriptor()
{
    QJsonArray games;
    for(GameDetails game : matchDetails.games) {
        games.append(QJsonObject({
            { "deck", deckToJsonObject(game.playerDeck) },
            { "duration", game.duration },
            { "matchWinCondition", game.gameInfo.winCondition },
            { "opponentRevealedCards", cardsToJsonArray(game.opponentRevealedDeck.currentCards()) },
            { "result", resultSpecToJsonObject(game.resultSpec) }
        }));
    }
    return games;
}

QJsonObject UntappedMatchDescriptor::getMatchPlayerDescriptor()
{
    return QJsonObject({
       { "name", matchDetails.player.name() },
       { "accountId", matchDetails.player.accountId() },
       { "teamId", matchDetails.player.teamId() },
       { "systemSeatId", matchDetails.player.seatId() },
       { "preMatchRankInfo", QJsonObject({
           { "rankClass", matchDetails.playerCurrentRankInfo.rankClass() },
           { "tier", intToJsonValue(matchDetails.playerCurrentRankInfo.rankTier()) },
           { "step", intToJsonValue(matchDetails.playerCurrentRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intToJsonValue(matchDetails.playerCurrentRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleToJsonValue(matchDetails.playerCurrentRankInfo.mythicPercentile()) }
       })},
       { "postMatchRankInfo", QJsonObject({
           { "rankClass", matchDetails.playerOldRankInfo.rankClass() },
           { "tier", intToJsonValue(matchDetails.playerOldRankInfo.rankTier()) },
           { "step", intToJsonValue(matchDetails.playerOldRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intToJsonValue(matchDetails.playerOldRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleToJsonValue(matchDetails.playerOldRankInfo.mythicPercentile()) }
       })}
    });
}

QJsonArray UntappedMatchDescriptor::getMatchOpponentsDescriptor()
{
    return QJsonArray({
        QJsonObject({
            { "name", matchDetails.opponent.name() },
            { "accountId", matchDetails.opponent.accountId() },
            { "teamId", matchDetails.opponent.teamId() },
            { "systemSeatId", matchDetails.opponent.seatId() },
            { "preMatchRankInfo", QJsonObject({
                { "rankClass", matchDetails.opponentRankInfo.rankClass() },
                { "tier", intToJsonValue(matchDetails.opponentRankInfo.rankTier()) },
                { "step", intToJsonValue(matchDetails.opponentRankInfo.rankStep()) },
                { "mythicLeaderboardPlace", matchDetails.opponentRankInfo.mythicLeaderboardPlace() },
                { "mythicPercentile", matchDetails.opponentRankInfo.mythicPercentile() }
            })},
            { "postMatchRankInfo", QJsonValue::Null }
        })
    });
}

QJsonObject UntappedMatchDescriptor::getMatchEventDescriptor(EventPlayerCourse eventPlayerCourse)
{
    return QJsonObject({
       { "name", matchDetails.eventId },
       { "maxWins", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.maxWins) },
       { "maxLosses", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.maxLosses) },
       { "currentWins", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.currentWins) },
       { "currentLosses", eventCourseIntToJsonValue(eventPlayerCourse.eventId, eventPlayerCourse.currentLosses) },
       { "processedMatchIds", eventPlayerCourse.eventId == matchDetails.eventId
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
    if (eventId != matchDetails.eventId) {
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
