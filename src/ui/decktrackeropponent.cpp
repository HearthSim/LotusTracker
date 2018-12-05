#include "decktrackeropponent.h"
#include "../macros.h"

#include <QApplication>
#include <QPoint>

DeckTrackerOpponent::DeckTrackerOpponent(QWidget *parent) : DeckTrackerBase(parent)
{
    applyCurrentSettings();
}

DeckTrackerOpponent::~DeckTrackerOpponent()
{

}

void DeckTrackerOpponent::applyCurrentSettings()
{
    uiPos = APP_SETTINGS->getDeckTrackerOpponentPos(uiWidth);
    uiScale = APP_SETTINGS->getDeckTrackerOpponentScale();
    lastUiScale = uiScale;
    DeckTrackerBase::onScaleChanged();
}

int DeckTrackerOpponent::getDeckNameYPosition()
{
    return uiPos.y() - titleHeight - 5;
}

QString DeckTrackerOpponent::onGetDeckColorIdentity()
{
    return deck.colorIdentity(false, true);
}

void DeckTrackerOpponent::onPositionChanged()
{
    APP_SETTINGS->setDeckTrackerOpponentPos(uiPos);
}

void DeckTrackerOpponent::onScaleChanged()
{
    DeckTrackerBase::onScaleChanged();
    if (uiScale > lastUiScale) {
        uiPos -= QPoint(10, 0);
    } else {
        uiPos += QPoint(10, 0);
    }
    APP_SETTINGS->setDeckTrackerOpponentScale(uiScale);
    lastUiScale = uiScale;
}

void DeckTrackerOpponent::afterPaintEvent(QPainter &painter)
{
    UNUSED(painter);
}

void DeckTrackerOpponent::setEventId(QString eventId)
{
    this->eventId = eventId;
}

void DeckTrackerOpponent::reset()
{
    deck.clear();
    deck.updateTitle("");
    eventId = "";
    update();
}

void DeckTrackerOpponent::onOpponentPutInLibraryCard(Card* card)
{
    deck.drawCard(card); //remove a card from opponent current deck on screen
}

void DeckTrackerOpponent::onOpponentPlayCard(Card* card)
{
    insertCard(card);
}

void DeckTrackerOpponent::onOpponentDiscardCard(Card* card)
{
    insertCard(card);
}

void DeckTrackerOpponent::onOpponentDiscardFromLibraryCard(Card* card)
{
    insertCard(card);
}

void DeckTrackerOpponent::onOpponentPutOnBattlefieldCard(Card* card)
{
    insertCard(card);
}

void DeckTrackerOpponent::insertCard(Card* card)
{
    deck.insertCard(card);
    blinkCard(card);
    QMap<Card*, int> deckCards = deck.currentCards();
    QMap<Card*, int> nonLandCards;
    for (Card* card : deckCards.keys()) {
        if (card->isLand) {
            continue;
        }
        nonLandCards[card] = deckCards[card];
    }
    if (eventId.contains("constructed") && nonLandCards.size() >= 2) {
        QString deckArchtecture = LOTUS_TRACKER->mtgDecksArch->
                findDeckArchitecture(deckCards);
        deck.updateTitle(deckArchtecture);
    }
}
