#include "decktrackeropponent.h"
#include "../macros.h"

#include <QApplication>
#include <QPoint>

DeckTrackerOpponent::DeckTrackerOpponent(QWidget *parent) : DeckTrackerBase(parent)
{
    uiPos.setX(cardHoverWidth + 10);
    applyCurrentSettings();
}

DeckTrackerOpponent::~DeckTrackerOpponent()
{

}

void DeckTrackerOpponent::applyCurrentSettings()
{
    move(APP_SETTINGS->getDeckTrackerOpponentPos(uiWidth, cardHoverWidth));
    uiScale = APP_SETTINGS->getDeckTrackerOpponentScale();
    lastUiScale = uiScale;
    DeckTrackerBase::onScaleChanged();
}

int DeckTrackerOpponent::getDeckNameYPosition()
{
    return uiPos.y() - titleHeight - 7;
}

int DeckTrackerOpponent::getHoverCardXPosition()
{
    return uiPos.x() - cardHoverWidth - 10;
}

QString DeckTrackerOpponent::getDeckColorIdentity()
{
    return deck.colorIdentity(false, true);
}

void DeckTrackerOpponent::onPositionChanged()
{
    APP_SETTINGS->setDeckTrackerOpponentPos(pos());
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

void DeckTrackerOpponent::onReceiveEventInfo(QString name, QString type)
{
    UNUSED(name);
    eventType = type;
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
    if (eventType == "Constructed" && nonLandCards.size() >= 3) {
        QString deckArchtecture = LOTUS_TRACKER->mtgDecksArch->
                findDeckArchitecture(deckCards);
        deck.updateTitle(deckArchtecture);
    }
}
