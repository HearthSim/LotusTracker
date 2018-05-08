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

void DeckTrackerOpponent::onOpponentPlayCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
}
