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
    currentSource = APP_SETTINGS->getDeckOverlayDraftSource();
    DeckOverlayBase::onScaleChanged();
}

QList<Card *> DeckOverlayDraft::getDeckCardsSorted()
{
    QList<Card*> sortedDeckCards(deck.currentCards().keys());
    std::sort(std::begin(sortedDeckCards), std::end(sortedDeckCards), [](Card*& lhs, Card*& rhs) {
        QList<QString> rarities = { "mythic", "rare", "uncommon", "common" };
        QList<QString> colors = { "w", "u", "b", "r", "g", "d", "m", "a", "c" };
        QString lhsColors = lhs->borderColorIdentityAsString();
        QString lhsDualFirstColor = "";
        if (lhsColors.length() > 1) {
            lhsColors = "d";
            lhsDualFirstColor = lhs->colorIdentity.first();
        }
        QString rhsDualFirstColor = "";
        QString rhsColors = rhs->borderColorIdentityAsString();
        if (rhsColors.length() > 1) {
            rhsColors = "d";
            lhsDualFirstColor = rhs->colorIdentity.first();
        }
        return std::make_tuple(lhs->isBasicLand(), rarities.indexOf(lhs->rarity), lhs->isLand,
                               colors.indexOf(lhsColors), lhsDualFirstColor, lhs->cmc) <
                std::make_tuple(rhs->isBasicLand(), rarities.indexOf(rhs->rarity), lhs->isLand,
                                colors.indexOf(rhsColors), rhsDualFirstColor, rhs->cmc);
    });
    return sortedDeckCards;
}

int DeckOverlayDraft::getDeckNameYPosition()
{
    return uiPos.y() - titleHeight - 7;
}

int DeckOverlayDraft::getHoverCardXPosition()
{
    QString rank = getHoverCardRank();
    QFontMetrics cardMetrics(cardFont);
    int rankWidth = cardMetrics.width(rank);
    return uiPos.x() + uiWidth + rankWidth + 15 + uiScale * 3;
}

int DeckOverlayDraft::cardHoverMarginBottom(QPainter &painter)
{
    int lines = 0;
    float ratio = isShowCardManaCostEnabled ? 4 : 3.5f;
    int heightRaw = static_cast<int>(uiWidth/ratio);
    if (currentSource == "lsv") {
        int width = static_cast<int>(uiWidth * 1.5);

        painter.setFont(cardFont);
        float descWidth = painter.fontMetrics().width(hoverCard->lsvDesc);
        float lineWidth = width - rankDescTextMargin * 2;
        float linesRaw = descWidth / lineWidth;
        lines = qCeil(static_cast<qreal>(linesRaw));
    }
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
    int rankMargin = 2 + uiScale;
    QFontMetrics cardMetrics(rankFont);
    int rankTextHeight = cardMetrics.ascent() - cardMetrics.descent();
    int rankTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    painter.setFont(rankFont);
    QString rank = currentSource == "lsv" ? card->lsvRank : card->draftsimRank;
    if (rank.length() == 1) {
        rank += ".0";
    }
    int rankWidth = painter.fontMetrics().width(rank);
    int rankX = uiPos.x() + uiWidth + 2;
    int rankY = cardBGY + getCardHeight()/2 - rankTextHeight/2;
#if defined Q_OS_WIN
    rankY -= 1;
#endif

    int rankBGWidth = rankWidth + rankMargin * 4;
    int rankBGX = rankX - rankMargin * 3;
    QRect descRect(rankBGX, cardBGY, rankBGWidth, getCardHeight() - 1);
    painter.setPen(bgPen);
    painter.setBrush(QBrush(QColor(70, 70, 70, 200)));
    painter.drawRoundedRect(descRect, cornerRadius, cornerRadius);

    QPen rankPen = QPen(Qt::yellow);
    if (rank < "2.0") {
        rankPen = QPen(Qt::white);
    } else if (rank > "3.0") {
        rankPen = QPen(Qt::green);
    }
    drawText(painter, cardFont, rankPen, rank, rankTextOptions, true,
             rankX, rankY, rankTextHeight, rankWidth);
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
    int width = static_cast<int>(uiWidth * 1.6);
    QString desc = "";
    if (currentSource == "lsv") {
        desc = QString("\t\t\t\t\tChannelFireball LSV Tier: %1\n%2")
                .arg(getHoverCardRank()).arg(hoverCard->lsvDesc);
    } else {
        desc = QString("\t\t\t\t\tDraftSim Tier: %1").arg(getHoverCardRank());
    }

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
    QRect textRect(descX + rankDescTextMargin, descY,
                   width - rankDescTextMargin * 2, height);
    painter.setPen(QPen(QColor(255, 255, 150, 250)));
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
    pickedCards.clear();
    playerCollection.clear();
    update();
}

void DeckOverlayDraft::setPlayerCollection(QMap<int, int> ownedCards)
{
    playerCollection = ownedCards;
    if (!availablePicks.isEmpty()) {
        udpateAvailableCardsList(availablePicks, pickedCards);
    }
}

void DeckOverlayDraft::onDraftStatus(QList<Card *> availablePicks, QList<Card *> pickedCards)
{
    this->availablePicks = availablePicks;
    this->pickedCards = pickedCards;
    if (playerCollection.keys().isEmpty()) {
        emit sgnRequestPlayerCollection();
    }
    udpateAvailableCardsList(availablePicks, pickedCards);
}

void DeckOverlayDraft::onSourceChanged(QString source)
{
    currentSource = source;
    update();
}

QString DeckOverlayDraft::getHoverCardRank()
{
    if (currentSource == "lsv") {
        return hoverCard->lsvRank;
    } else {
        return hoverCard->draftsimRank;
    }
}

void DeckOverlayDraft::udpateAvailableCardsList(QList<Card*> availablePicks, QList<Card*> pickedCards)
{
    deck.clear();
    for (Card* card : availablePicks) {
        int qtdPicked = 0;
        for (Card* cardPicked : pickedCards) {
            if (cardPicked->mtgaId == card->mtgaId) {
                qtdPicked++;
            }
        }
        int qtdOwned = playerCollection[card->mtgaId] + qtdPicked;
        if (qtdOwned > 4) {
            qtdOwned = 4;
        }
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
