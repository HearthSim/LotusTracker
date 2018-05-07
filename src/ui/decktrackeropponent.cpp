#include "decktrackeropponent.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>

DeckTrackerOpponent::DeckTrackerOpponent(QWidget *parent) : DeckTrackerBase(parent)
{
    QRect screen = QApplication::desktop()->screenGeometry();
    int x = screen.width() - uiWidth - 10;
    uiPos = QPoint(x, uiPos.y());
}

DeckTrackerOpponent::~DeckTrackerOpponent()
{

}

void DeckTrackerOpponent::onOpponentPlayCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
}
