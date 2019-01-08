#include "deckoverlayopponent.h"
#include "../macros.h"

#include <QApplication>
#include <QPoint>

DeckOverlayOpponent::DeckOverlayOpponent(QWidget *parent) : DeckOverlayBase(parent)
{
    uiPos.setX(cardHoverWidth + 10);
    applyCurrentSettings();
}

DeckOverlayOpponent::~DeckOverlayOpponent()
{

}

void DeckOverlayOpponent::applyCurrentSettings()
{
    move(APP_SETTINGS->getDeckOverlayOpponentPos(uiWidth, cardHoverWidth));
    uiScale = APP_SETTINGS->getDeckOverlayOpponentScale();
    lastUiScale = uiScale;
    DeckOverlayBase::onScaleChanged();
}

int DeckOverlayOpponent::getDeckNameYPosition()
{
    return uiPos.y() - titleHeight - 7;
}

int DeckOverlayOpponent::getHoverCardXPosition()
{
    return uiPos.x() - cardHoverWidth - 10;
}

QString DeckOverlayOpponent::getDeckColorIdentity()
{
    return deck.colorIdentity(false, true);
}

void DeckOverlayOpponent::onPositionChanged()
{
    APP_SETTINGS->setDeckOverlayOpponentPos(pos());
}

void DeckOverlayOpponent::onScaleChanged()
{
    DeckOverlayBase::onScaleChanged();
    if (uiScale > lastUiScale) {
        uiPos -= QPoint(10, 0);
    } else {
        uiPos += QPoint(10, 0);
    }
    APP_SETTINGS->setDeckOverlayOpponentScale(uiScale);
    lastUiScale = uiScale;
}

void DeckOverlayOpponent::afterPaintEvent(QPainter &painter)
{
    UNUSED(painter);
}

void DeckOverlayOpponent::setEventId(QString eventId)
{
    this->eventId = eventId;
}

void DeckOverlayOpponent::reset()
{
    deck.clear();
    deck.updateTitle("");
    eventId = "";
    update();
}

void DeckOverlayOpponent::onReceiveEventInfo(QString name, QString type)
{
    UNUSED(name);
    eventType = type;
}

void DeckOverlayOpponent::onOpponentPutInLibraryCard(Card* card)
{
    deck.drawCard(card); //remove a card from opponent current deck on screen
}

void DeckOverlayOpponent::onOpponentPlayCard(Card* card)
{
    insertCard(card);
}

void DeckOverlayOpponent::onOpponentDiscardCard(Card* card)
{
    insertCard(card);
}

void DeckOverlayOpponent::onOpponentDiscardFromLibraryCard(Card* card)
{
    insertCard(card);
}

void DeckOverlayOpponent::onOpponentPutOnBattlefieldCard(Card* card)
{
    insertCard(card);
}

void DeckOverlayOpponent::insertCard(Card* card)
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
