#include "matchdetails.h"
#include "../macros.h"

MatchDetails::MatchDetails(QString matchId, QString eventId, RankInfo opponentRankInfo):
    matchId(matchId), eventId(eventId), player(MatchPlayer()), playerCurrentRankInfo(RankInfo()),
    opponent(MatchPlayer()), opponentRankInfo(opponentRankInfo), resultSpec(ResultSpec()),
    playerMatchWins(false), playerGameWins(0), playerGameLoses(0)
{
    games.clear();
}

void MatchDetails::createNewGame(GameInfo gameInfo)
{
    if (games.size() == 0 || currentGame().isCompleted) {
        games << GameDetails(gameInfo);
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
