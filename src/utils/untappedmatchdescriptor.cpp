#include "untappedmatchdescriptor.h"
#include "../macros.h"

#include <QtMath>

UntappedMatchDescriptor::UntappedMatchDescriptor(QObject *parent) : QObject(parent)
{

}

QJsonDocument UntappedMatchDescriptor::prepareNewDescriptor(MatchDetails matchDetails,
                                                            QString timestamp, QString uploadToken,
                                                            EventPlayerCourse eventPlayerCourse)
{
    this->matchDetails = matchDetails;
    return QJsonDocument(
        QJsonObject({
            { "timestamp", timestamp },
            { "summarizedMessageCount", matchDetails.summarizedMessage },
            { "client", QString("lt-%1").arg(qApp->applicationVersion()) },
            { "mtgaVersion", LOTUS_TRACKER->mtgArena->getClientVersion() },
            { "uploadToken", uploadToken },
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
    qint64 matchStartTime = matchDetails.games.first().startTime;
    for(GameDetails game : matchDetails.games) {
        games.append(QJsonObject({
            { "deck", deckToJsonObject(game.playerDeck) },
            { "duration", game.duration },
            { "number", game.gameInfo.number },
            { "relativeStartTime", qFloor((game.startTime - matchStartTime) / 1000) },
            { "turns", game.turns },
            { "initialActivePlayer", game.activePlayer },
            { "initialDecisionPlayer", game.decisionPlayer },
            { "superFormat", stringOrNullJsonValue(game.gameInfo.superFormat) },
            { "type", stringOrNullJsonValue(game.gameInfo.type) },
            { "variant", stringOrNullJsonValue(game.gameInfo.variant) },
            { "matchWinCondition", stringOrNullJsonValue(game.gameInfo.winCondition) },
            { "mulliganType", stringOrNullJsonValue(game.gameInfo.mulliganType) },
            { "playerRevealedCards", getPlayerRevealedCards(game.playerDeck) },
            { "opponentRevealedCards", QJsonArray({
                  cardsToJsonArray(game.opponentRevealedDeck.currentCards())
            })},
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
           { "tier", intOrNullJsonValue(matchDetails.playerCurrentRankInfo.rankTier()) },
           { "step", intOrNullJsonValue(matchDetails.playerCurrentRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intOrNullJsonValue(matchDetails.playerCurrentRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleOrNullToJsonValue(matchDetails.playerCurrentRankInfo.mythicPercentile()) }
       })},
       { "postMatchRankInfo", QJsonObject({
           { "rankClass", matchDetails.playerOldRankInfo.rankClass() },
           { "tier", intOrNullJsonValue(matchDetails.playerOldRankInfo.rankTier()) },
           { "step", intOrNullJsonValue(matchDetails.playerOldRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intOrNullJsonValue(matchDetails.playerOldRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleOrNullToJsonValue(matchDetails.playerOldRankInfo.mythicPercentile()) }
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
                { "tier", intOrNullJsonValue(matchDetails.opponentRankInfo.rankTier()) },
                { "step", intOrNullJsonValue(matchDetails.opponentRankInfo.rankStep()) },
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

QJsonArray UntappedMatchDescriptor::getPlayerRevealedCards(Deck deck)
{
    QMap<Card*, int> revealedCards = deck.cards();
    for (Card* card : deck.currentCards().keys()) {
        revealedCards[card] -= deck.currentCards()[card];
    }
    return cardsToJsonArray(revealedCards);
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
    return intOrNullJsonValue(value);
}

QJsonValue UntappedMatchDescriptor::stringOrNullJsonValue(QString value)
{
    if (value.isEmpty()) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}

QJsonValue UntappedMatchDescriptor::intOrNullJsonValue(int value)
{
    if (value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}

QJsonValue UntappedMatchDescriptor::doubleOrNullToJsonValue(double value)
{
    if (value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}
