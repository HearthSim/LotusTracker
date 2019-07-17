#include "matchinfo.h"
#include "../macros.h"

MatchInfo::MatchInfo(QString matchId, QString eventId, RankInfo opponentRankInfo):
    matchId(matchId), eventId(eventId), player(MatchPlayer()), playerCurrentRankInfo(RankInfo()),
    opponent(MatchPlayer()), opponentRankInfo(opponentRankInfo), resultSpec(ResultSpec()),
    playerMatchWins(false), playerGameWins(0), playerGameLoses(0)
{
    games.clear();
}

void MatchInfo::createNewGame(GameDetails details)
{
    if (games.size() == 0 || currentGame().isCompleted) {
        games << GameInfo(details);
    }
}

GameInfo& MatchInfo::currentGame()
{
    if (games.size() == 0) {
        LOGW("Invalid game state");
        games << GameInfo(GameDetails());
    }
    return games.last();
}

QMap<Card*, int> MatchInfo::getOpponentMatchesCards() {
    QMap<Card*, int> opponentMatchCards;
    for (GameInfo gameInfo : games) {
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
QString MatchInfo::getOpponentDeckArch()
{
    QMap<Card*, int> cards = getOpponentMatchesCards();
    return LOTUS_TRACKER->mtgDecksArch->findDeckArchitecture(cards);
}

QString MatchInfo::getOpponentDeckColorIdentity()
{
    QMap<Card*, int> cards = getOpponentMatchesCards();
    return Deck::calcColorIdentity(cards, true);
}
