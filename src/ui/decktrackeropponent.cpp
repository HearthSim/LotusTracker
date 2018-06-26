#include "decktrackeropponent.h"
#include "../macros.h"

#include <QApplication>
#include <QPoint>

DeckTrackerOpponent::DeckTrackerOpponent(QWidget *parent) : DeckTrackerBase(parent)
{
    uiPos = APP_SETTINGS->getDeckTrackerOpponentPos(uiWidth);
    uiScale = APP_SETTINGS->getDeckTrackerOpponentScale();
}

DeckTrackerOpponent::~DeckTrackerOpponent()
{

}

void DeckTrackerOpponent::onPositionChanged()
{
    APP_SETTINGS->setDeckTrackerOpponentPos(uiPos);
}

void DeckTrackerOpponent::onScaleChanged()
{
    APP_SETTINGS->setDeckTrackerOpponentScale(uiScale);
}

void DeckTrackerOpponent::afterPaintEvent(QPainter &painter)
{
    UNUSED(painter);
}

void DeckTrackerOpponent::reset()
{
    deck.cards.clear();
}

void DeckTrackerOpponent::onOpponentPutInLibraryCard(Card* card)
{
    deck.drawCard(card); //remove a card from opponent current deck on screen
}

void DeckTrackerOpponent::onOpponentPlayCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
}

void DeckTrackerOpponent::onOpponentDiscardCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
}

void DeckTrackerOpponent::onOpponentDiscardFromLibraryCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
}

void DeckTrackerOpponent::onOpponentPutOnBattlefieldCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
}
