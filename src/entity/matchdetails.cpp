#include "matchdetails.h"
#include "../macros.h"

MatchDetails::MatchDetails(QString matchId, QString eventId, QMap<Card*, int> playerCommanders,
                           RankInfo opponentRankInfo, QMap<Card*, int> opponentCommanders):
    nextGameActivePlayer(0), nextGameDecisionPlayer(0), matchId(matchId), eventId(eventId),
    player(MatchPlayer()), playerCommanders(playerCommanders), playerCurrentRankInfo(RankInfo()),
    opponent(MatchPlayer()), opponentCommanders(opponentCommanders), opponentRankInfo(opponentRankInfo),
    resultSpec(ResultSpec()), playerMatchWins(false), playerGameWins(0), playerGameLoses(0),
    seasonOrdinal(0), summarizedMessage(0)
{
    games.clear();
}

void MatchDetails::createNewGame(GameInfo gameInfo)
{
    if (games.size() == 0 || currentGame().isCompleted) {
        games << GameDetails(gameInfo, nextGameActivePlayer, nextGameDecisionPlayer);
        nextGameActivePlayer = 0;
        nextGameDecisionPlayer = 0;
        return;
    }
    if (currentGame().gameInfo.type.isEmpty()) {
        currentGame().gameInfo = gameInfo;
    }
}

GameDetails& MatchDetails::currentGame()
{
    if (games.size() == 0) {
        LOGW("Invalid game state");
        games << GameDetails(GameInfo());
    }
    return games.last();
}

QMap<Card*, int> MatchDetails::getOpponentMatchesCards() {
    QMap<Card*, int> opponentMatchCards;
    for (GameDetails gameInfo : games) {
        QMap<Card*, int> gameOpponentCards = gameInfo.opponentRevealedDeck.currentCards();
        for (Card* card : gameOpponentCards.keys()){
            if (opponentMatchCards.keys().contains(card)) {
                opponentMatchCards[card] += gameOpponentCards[card];
            } else {
                opponentMatchCards[card] = gameOpponentCards[card];
            }
        }
    }
    return opponentMatchCards;
}
QString MatchDetails::getOpponentDeckArch()
{
    QMap<Card*, int> cards = getOpponentMatchesCards();
    return LOTUS_TRACKER->mtgDecksArch->findDeckArchitecture(cards);
}

QString MatchDetails::getOpponentDeckColorIdentity()
{
    QMap<Card*, int> cards = getOpponentMatchesCards();
    return Deck::calcColorIdentity(cards, true);
}

void MatchDetails::onActivePlayer(int player)
{
    if (games.size() == 0 && nextGameActivePlayer == 0) {
        nextGameActivePlayer = player;
    }
    if (games.size() > 0 && currentGame().activePlayer == 0) {
        currentGame().activePlayer = player;
        nextGameActivePlayer = 0;
    }
}

void MatchDetails::onDecisionPlayer(int player)
{
    if (games.size() == 0 && nextGameDecisionPlayer == 0) {
        nextGameDecisionPlayer = player;
    }
    if (games.size() > 0 && currentGame().decisionPlayer == 0) {
        currentGame().decisionPlayer = player;
        nextGameDecisionPlayer = 0;
    }
}
