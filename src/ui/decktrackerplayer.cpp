#include "decktrackerplayer.h"
#include "../macros.h"

DeckTrackerPlayer::DeckTrackerPlayer(QWidget *parent) : DeckTrackerBase(parent)
{
    uiPos = QPoint(10, 10);
}

DeckTrackerPlayer::~DeckTrackerPlayer()
{

}

void DeckTrackerPlayer::onPlayerDeckSelected(Deck deck)
{
    this->deck = deck;
    LOGD(QString("Loading deck %1").arg(deck.name));
}

void DeckTrackerPlayer::onPlayerDrawCard(Card* card)
{
    if (deck.drawCard(card)) {
        blinkCard(card);
    }
}
