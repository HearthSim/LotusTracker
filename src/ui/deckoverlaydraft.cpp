#include "deckoverlaydraft.h"
#include "../macros.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QPoint>
#include <QToolTip>
#include <QtMath>

DeckOverlayDraft::DeckOverlayDraft(QWidget *parent) : DeckOverlayBase(parent)
{
    rankDescTextMargin = 10;
    rankFont.setBold(true);
    applyCurrentSettings();
}

DeckOverlayDraft::~DeckOverlayDraft()
{

}

void DeckOverlayDraft::applyCurrentSettings()
{
    move(APP_SETTINGS->getDeckOverlayDraftPos(uiWidth));
    uiScale = APP_SETTINGS->getDeckOverlayDraftScale();
    DeckOverlayBase::onScaleChanged();
}

QList<Card *> DeckOverlayDraft::getDeckCardsSorted()
{
    QList<Card*> sortedDeckCards(deck.currentCards().keys());
    std::sort(std::begin(sortedDeckCards), std::end(sortedDeckCards), [](Card*& lhs, Card*& rhs) {
        QList<QString> rarities = { "common", "uncommon", "rare", "mythic" };
        QList<QString> colors = { "c", "a", "m", "d", "g", "r", "b", "u", "w" };
        QString lhsColors = lhs->borderColorIdentityAsString();
        if (lhsColors.length() > 1) {
            lhsColors = "d";
        }
        QString rhsColors = rhs->borderColorIdentityAsString();
        if (rhsColors.length() > 1) {
            rhsColors = "d";
        }
        return std::make_tuple(rarities.indexOf(lhs->rarity), colors.indexOf(lhsColors), lhs->name) >
                std::make_tuple(rarities.indexOf(rhs->rarity), colors.indexOf(rhsColors), rhs->name);
    });
    return sortedDeckCards;
}

int DeckOverlayDraft::getDeckNameYPosition()
{
    return uiPos.y() - titleHeight - 7;
}

int DraftOverlay::getHoverCardXPosition()
{
    return uiPos.x() + uiWidth + 10;
}

QString DeckOverlayDraft::getDeckColorIdentity()
{
    return deck.colorIdentity(false, true);
}

QString DeckOverlayDraft::cardQtdFormat()
{
    return "%1/4 ";
}

bool DeckOverlayDraft::useGrayscaleForZeroQtd()
{
    return false;
}

void DeckOverlayDraft::onPositionChanged()
{
    APP_SETTINGS->setDeckOverlayDraftPos(pos());
}

void DeckOverlayDraft::onScaleChanged()
{
    DeckOverlayBase::onScaleChanged();
    APP_SETTINGS->setDeckOverlayDraftScale(uiScale);
}

void DeckOverlayDraft::beforeDrawCardEvent(QPainter &painter, Card *card, int cardBGY)
{
    int rankMargin = 10 + uiScale/2;
    QFontMetrics cardMetrics(rankFont);
    int rankTextHeight = cardMetrics.ascent() - cardMetrics.descent();
    int rankTextOptions = Qt::AlignRight | Qt::AlignVCenter | Qt::TextDontClip;
    painter.setFont(rankFont);
    QString rank = card->lsvRank;
    int rankWidth = painter.fontMetrics().width(rank);
    int rankX = uiPos.x() + uiWidth;
    int rankY = cardBGY + getCardHeight()/2 - rankTextHeight/2;
#if defined Q_OS_WIN
    rankY -= 1;
#endif

    int rankBGWidth = rankWidth + rankMargin + rankMargin;
    int rankBGX = rankX - rankMargin;
    QRect descRect(rankBGX, cardBGY, rankBGWidth, getCardHeight() - 1);
    painter.setPen(bgPen);
    painter.setBrush(QBrush(QColor(70, 70, 70, 200)));
    painter.drawRoundedRect(descRect, cornerRadius, cornerRadius);

    QPen rankPen = QPen(Qt::white);
    drawText(painter, cardFont, rankPen, rank, rankTextOptions, true,
             rankX + uiScale, rankY, rankTextHeight, rankWidth);
}

}

void DeckOverlayDraft::afterPaintEvent(QPainter &painter)
{
    // Preferences button
    int buttonSize = 16 + static_cast<int> (uiScale * 1);
    int buttonMarginX = 3;
    int buttonMarginY = 2;
    int preferencesButtonY = uiPos.y() + buttonMarginY;
    QImage settings(":res/preferences.png");
    QImage settingsScaled = settings.scaled(buttonSize, buttonSize,
                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int settingsPlusX = uiPos.x() + uiWidth - buttonSize - buttonMarginX;
    painter.drawImage(settingsPlusX, preferencesButtonY, settingsScaled);
    preferencesButton = QRect(settingsPlusX, preferencesButtonY, buttonSize, buttonSize);
}

void DeckOverlayDraft::reset()
{
    deck.clear();
    deck.updateTitle("");
    availablePicks.clear();
    playerCollection.clear();
    update();
}

void DeckOverlayDraft::setPlayerCollection(QMap<int, int> ownedCards)
{
    playerCollection = ownedCards;
    if (!availablePicks.isEmpty()) {
        udpateAvailableCardsList(availablePicks);
    }
}

void DeckOverlayDraft::onDraftStatus(QList<Card *> availablePicks, QList<Card *> pickedCards)
{
    UNUSED(pickedCards);
    this->availablePicks = availablePicks;
    if (playerCollection.keys().isEmpty()) {
        emit sgnRequestPlayerCollection();
    }
    udpateAvailableCardsList(availablePicks);
}

void DeckOverlayDraft::udpateAvailableCardsList(QList<Card *> availablePicks)
{
    deck.clear();
    for (Card* card : availablePicks) {
        int qtdOwned = playerCollection[card->mtgaId];
        deck.setCardQtd(card, qtdOwned);
    }
    update();
}

void DeckOverlayDraft::onHoverMove(QHoverEvent *event)
{
    showingTooltip = false;
    if (preferencesButton.contains(event->pos())) {
        showingTooltip = true;
        QToolTip::showText(event->pos(), tr("Settings"));
    }
    DeckOverlayBase::onHoverMove(event);
}

void DeckOverlayDraft::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (preferencesButton.contains(event->pos())) {
        return;
    }
    DeckOverlayBase::mousePressEvent(event);
}

void DeckOverlayDraft::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        return;
    }
    if (preferencesButton.contains(event->pos())) {
        hideCardOnHover();
        LOTUS_TRACKER->showPreferencesScreen();
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Preferences");
        return;
    }
    DeckOverlayBase::mouseReleaseEvent(event);
}
