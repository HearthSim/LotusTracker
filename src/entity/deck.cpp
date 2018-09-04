#include "deck.h"
#include "../macros.h"
#include "../transformations.h"

Deck::Deck(QString id, QString name, QMap<Card*, int> cards, QMap<Card*, int> sideboard)
    : id(id), name(name), showOnlyRemainingCards(false){
    cardsInitial = cards;
    cardsSideboard = sideboard;
    for (Card *card : cards.keys()) {
        cardsCurrent[card] = cards[card];
    }

    _arch = cards.isEmpty() ? "" : ARENA_TRACKER->mtgDecksArch->
                              findDeckArchitecture(cards);
    _colorIdentity = Deck::calcColorIdentity(cards, false);
}

QString Deck::arch()
{
    return _arch;
}

QMap<Card*, int> Deck::cards(bool ignoreCardsWithSideboard)
{
    if (!ignoreCardsWithSideboard && !cardsInitialWithSideboard.isEmpty()) {
        return cardsInitialWithSideboard;
    } else {
        return cardsInitial;
    }
}

QMap<Card*, int> Deck::sideboard()
{
    return cardsSideboard;
}

QMap<Card*, int> Deck::currentCards()
{
    if (showOnlyRemainingCards) {
        QMap<Card*, int> onlyRemainingCards;
        for (Card* card : cardsCurrent.keys()) {
            int qtd = cardsCurrent[card];
            if (qtd > 0) {
                onlyRemainingCards[card] = qtd;
            }
        }
        return onlyRemainingCards;
    }
    return cardsCurrent;
}

int Deck::totalCards()
{
    int totalCards = 0;
    for (Card *card : cardsCurrent.keys()) {
        totalCards += cardsCurrent[card];
    }
    return totalCards;
}

int Deck::totalCardsLand()
{
    int totalLandCards = 0;
    for (Card *card : cardsCurrent.keys()) {
        if (card->isLand) {
            totalLandCards += cardsCurrent[card];
        }
    }
    return totalLandCards;
}

int Deck::totalCardsOfQtd(int qtd)
{
    int totalXCards = 0;
    for (Card *card : cardsCurrent.keys()) {
        if (cardsCurrent[card] == qtd && !card->isLand) {
            totalXCards += qtd;
        }
    }
    return totalXCards;
}

void Deck::updateCards(QMap<Card*, int> cards)
{
    cardsInitialWithSideboard = cards;
    reset();
}

void Deck::updateTitle(QString title)
{
    this->name = title;
}

void Deck::clear()
{
    cardsCurrent.clear();
}

void Deck::reset()
{
    for (Card *card : cards().keys()) {
        cardsCurrent[card] = cards()[card];
    }
}

bool Deck::isReseted()
{
    for (Card *card : cardsInitial.keys()) {
        if (cardsCurrent[card] != cardsInitial[card]) {
            return false;
        }
    }
    return true;
}

QString Deck::colorIdentity(bool useStartCalc, bool includeLands)
{
    if (useStartCalc) {
        return _colorIdentity;
    }
    return Deck::calcColorIdentity(cardsCurrent, includeLands);
}

bool Deck::drawCard(Card *card)
{
    if (cardsCurrent[card] > 0) {
        cardsCurrent[card] -= 1;
        return true;
    } else {
        return false;
    }
}

void Deck::insertCard(Card *card)
{
    if (cardsCurrent.keys().contains(card)) {
        cardsCurrent[card] += 1;
    } else {
        cardsCurrent[card] = 1;
    }
}

QString Deck::calcColorIdentity(QMap<Card*, int> cards, bool includeLands)
{
    QList<QChar> distinctManaSymbols;
    for (Card *card : cards.keys()) {
        if (card->isLand && !includeLands) {
            continue;
        }
        for (QChar symbol : card->borderColorIdentity) {
            if (symbol != QChar('a') && symbol != QChar('c') &&
                    symbol != QChar('m') && !distinctManaSymbols.contains(symbol)) {
                distinctManaSymbols << symbol;
            }
        }
    }
    if (distinctManaSymbols.size() >= 4) {
        return QString("m");
    } else {
        return Transformations::colorIdentityListToString(distinctManaSymbols);
    }
}
