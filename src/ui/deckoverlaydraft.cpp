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
        QList<QString> rarities = { "mythic", "rare", "uncommon", "common" };
        QList<QString> colors = { "w", "u", "b", "r", "g", "d", "m", "a", "c" };
        QString lhsColors = lhs->borderColorIdentityAsString();
        if (lhsColors.length() > 1) {
            lhsColors = "d";
        }
        QString rhsColors = rhs->borderColorIdentityAsString();
        if (rhsColors.length() > 1) {
            rhsColors = "d";
        }
        return std::make_tuple(rarities.indexOf(lhs->rarity), colors.indexOf(lhsColors), lhs->cmc) <
                std::make_tuple(rarities.indexOf(rhs->rarity), colors.indexOf(rhsColors), rhs->cmc);
    });
    return sortedDeckCards;
}

int DeckOverlayDraft::getDeckNameYPosition()
{
    return uiPos.y() - titleHeight - 7;
}

int DeckOverlayDraft::getHoverCardXPosition()
{
    QString rank = hoverCard->lsvRank;
    QFontMetrics cardMetrics(cardFont);
    int rankWidth = cardMetrics.width(rank);
    return uiPos.x() + uiWidth + rankWidth + 15 + uiScale * 3;
}

int DeckOverlayDraft::cardHoverMarginBottom(QPainter &painter)
{
    float ratio = isShowCardManaCostEnabled ? 4 : 3.5f;
    int heightRaw = static_cast<int>(uiWidth/ratio);
    int width = static_cast<int>(uiWidth * 1.5);

    painter.setFont(cardFont);
    float descWidth = painter.fontMetrics().width(hoverCard->lsvDesc);
    float lineWidth = width - rankDescTextMargin * 2;
    float linesRaw = descWidth / lineWidth;
    int lines = qCeil(static_cast<qreal>(linesRaw));
    int height = heightRaw / 3 * (lines + 2);
    return height;
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
    int rankWidth = painter.fontMetrics().width(QString(rank).replace("//", "/"));
    int rankX = uiPos.x() + uiWidth;
    int rankY = cardBGY + getCardHeight()/2 - rankTextHeight/2;
#if defined Q_OS_WIN
    rankY -= 1;
#endif

    int rankBGWidth = rankWidth + rankMargin * 2;
    int rankBGX = rankX - rankMargin;
    QRect descRect(rankBGX, cardBGY, rankBGWidth, getCardHeight() - 1);
    painter.setPen(bgPen);
    painter.setBrush(QBrush(QColor(70, 70, 70, 200)));
    painter.drawRoundedRect(descRect, cornerRadius, cornerRadius);

    QPen rankPen = QPen(Qt::white);
    drawText(painter, cardFont, rankPen, rank, rankTextOptions, true,
             rankX + uiScale, rankY, rankTextHeight, rankWidth);
}

void DeckOverlayDraft::drawHoverCard(QPainter &painter)
{
    if (!isShowCardOnHoverEnabled || hoverCard == nullptr) {
        return;
    }
    DeckOverlayBase::drawHoverCard(painter);
    // LVS Desc BG
    int bottomMargin = 10;
    int height = cardHoverMarginBottom(painter);
    int width = static_cast<int>(uiWidth * 2);
    QString desc = QString("\t\t\t\t\tLSV Rank: %1\n%2")
            .arg(hoverCard->lsvRank).arg(hoverCard->lsvDesc);

    QRect screen = QApplication::desktop()->screenGeometry();
    int descX = uiPos.x() + uiWidth + 2;
    int descY = uiPos.y() + (currentHoverPosition * getCardHeight()) + cardHoverHeight;
    if (pos().y() + descY > screen.height() - height - bottomMargin) {
        descY = screen.height() - height - bottomMargin - pos().y();
    }

    QRect descRect(descX, descY, width, height);
    painter.setPen(bgPen);
    painter.setBrush(QBrush(QColor(70, 70, 70, 250)));
    painter.drawRoundedRect(descRect, cornerRadius, cornerRadius);

    int descTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap;
    QRect textRect(descX + rankDescTextMargin, descY, width - rankDescTextMargin, height);
    painter.setPen(QPen(Qt::white));
    painter.drawText(textRect, descTextOptions, desc);
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
