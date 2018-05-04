#include "decktrackerui.h"
#include "../macros.h"

#include <QFontDatabase>
#include <tuple>

DeckTrackerUI::DeckTrackerUI(QObject *parent) : QObject(parent),
    uiHeight(0), uiWidth(160), cardBGSkin("mtga"), deckLoaded(false), mousePressed(false), mouseRelativePosition(QPoint())
{
    move(10, 10);
    coverPen = QPen(QColor(160, 160, 160));
    coverPen.setWidth(3);
    coverBrush = QBrush(QColor(70, 70, 70, 175));
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
    // Card
    cardFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    cardFont.setPointSize(9);
    cardFont.setBold(true);
    cardPen = QPen(Qt::black);
    // Title
    titleFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    titleFont.setPointSize(15);
    titleFont.setBold(true);
    titlePen = QPen(Qt::white);
}

DeckTrackerUI::~DeckTrackerUI()
{

}

int DeckTrackerUI::getWidth()
{
    return uiWidth;
}

void DeckTrackerUI::move(int x, int y)
{
    pos = QPoint(x, y);
}

void DeckTrackerUI::setupDeck(Deck _deck)
{
    deck = _deck;
    deckLoaded = true;
    LOGD(QString("Loading deck %1").arg(deck.name));
}

void DeckTrackerUI::paintEvent(QPainter &painter)
{
    drawCover(painter);
    painter.restore();
    if (deckLoaded) {
        drawDeckInfo(painter);
        drawDeckCards(painter);
    }
    painter.save();
}

void DeckTrackerUI::drawCover(QPainter &painter)
{
    // Cover BG
    QRect coverRect(pos.x(), pos.y(), uiWidth, uiWidth/4);
    painter.setPen(coverPen);
    painter.setBrush(coverBrush);
    painter.setClipRect(coverRect);
    painter.drawRoundedRect(coverRect, 10, 10);
    // Cover image
    bool coverImgLoaded = false;
    QImage coverImg;
    if (deckLoaded) {
        QString deckColorIdentity = deck.colorIdentity();
        coverImgLoaded = coverImg.load(QString(":/res/covers/%1.jpg").arg(deckColorIdentity));
    }
    if (!coverImgLoaded) {
        coverImg.load(":/res/covers/default.jpg");
    }
    QSize coverImgSize(coverRect.width() - 4, coverRect.height() - 4);
    QImage coverImgScaled = coverImg.scaled(coverImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(pos.x() + 2, pos.y() + 2, coverImgScaled);
    uiHeight = coverRect.height();
}

void DeckTrackerUI::drawDeckInfo(QPainter &painter)
{
    // Deck title
    QFontMetrics titleMetrics(titleFont);
    int titleHeight = titleMetrics.ascent() - titleMetrics.descent();
    int titleTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    drawText(painter, titleFont, titlePen, deck.name, titleTextOptions, true,
             pos.x() + 8, pos.y() + 8, titleHeight, uiWidth);
    // Deck identity
    int manaSize = 12;
    int manaX = pos.x() + 8;
    int manaY = pos.y() + uiHeight - manaSize - 5;
    QString deckColorIdentity = deck.colorIdentity();
    if (deckColorIdentity != "default" && deckColorIdentity != "m"){
        for (int i=0; i<deckColorIdentity.length(); i++) {
            QChar manaSymbol = deckColorIdentity.at(i);
            drawMana(painter, manaSymbol, manaSize, manaX, manaY);
            manaX += manaSize + 5;
        }
    }
}

void DeckTrackerUI::drawDeckCards(QPainter &painter)
{
    int cardListHeight = uiHeight;
    QSize cardBGImgSize(uiWidth, uiWidth/7);
    painter.setFont(cardFont);
    QFontMetrics cardMetrics(cardFont);
    int cardQtdOptions = Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip;
    int cardTextHeight = cardMetrics.ascent() - cardMetrics.descent();
    int cardTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    QList<Card*> deckCards(deck.cards.keys());
    std::sort(std::begin(deckCards), std::end(deckCards), [](Card*& lhs, Card*& rhs) {
        return std::make_tuple(lhs->isLand(), lhs->manaCostValue(), lhs->name) <
                std::make_tuple(rhs->isLand(), rhs->manaCostValue(), rhs->name);
    });
    for (Card* card : deckCards) {
        QList<QChar> cardManaList = card->manaColorIdentity();
        QString cardManaIdentity;
        for (QChar manaSymbol : cardManaList) {
            cardManaIdentity += manaSymbol;
        }
        // Card BG
        int cardBGY = pos.y() + cardListHeight;
        QImage cardBGImg;
        cardBGImg.load(QString(":/res/cards/%1/%2.png").arg(cardBGSkin).arg(cardManaIdentity));
        QImage cardBGImgScaled = cardBGImg.scaled(cardBGImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        painter.drawImage(pos.x(), cardBGY, cardBGImgScaled);
        // Card quantity
        QString cardQtd = QString("%1 ").arg(deck.cards[card]);
        int cardQtdWidth = painter.fontMetrics().width(cardQtd);
        int cardQtdX = pos.x() + 12;
        int cardQtdY = cardBGY + cardBGImgSize.height()/2 - cardTextHeight/2;
        drawText(painter, cardFont, cardPen, cardQtd, cardQtdOptions, false,
                 cardQtdX, cardQtdY, cardTextHeight, cardQtdWidth);
        // Card name
        drawText(painter, cardFont, cardPen, card->name, cardTextOptions, false,
                 cardQtdX + cardQtdWidth, cardQtdY, cardTextHeight, uiWidth);
        // Card mana
        int manaRightMargin = 7;
        int manaMargin = 2;
        int manaSize = 8;
        int manaCostWidth = card->manaCost.length() * (manaSize + manaMargin);
        int manaX = pos.x() + uiWidth - manaRightMargin - manaCostWidth;
        int manaY = cardBGY + cardBGImgSize.height()/2 - manaSize/2;
        for (QChar manaSymbol : card->manaCost) {
            drawMana(painter, manaSymbol, manaSize, manaX, manaY);
            manaX += manaSize + manaMargin;
        }
        cardListHeight += cardBGImgSize.height();
    }
    uiHeight += cardListHeight;
}

void DeckTrackerUI::drawText(QPainter &painter, QFont textFont, QPen textPen, QString text,
                        int textOptions, bool shadow, int textX, int textY, int textHeight, int textWidth)
{
    painter.setFont(textFont);
    if (shadow) {
        painter.setPen(QPen(Qt::black));
        painter.drawText(textX - 1, textY + 1, textWidth, textHeight, textOptions, text);
    }
    painter.setPen(textPen);
    painter.drawText(textX, textY, textWidth, textHeight, textOptions, text);
}

void DeckTrackerUI::drawMana(QPainter &painter, QChar manaSymbol, int manaSize, int manaX, int manaY)
{
    QImage manaImg;
    manaImg.load(QString(":/res/mana/%1.png").arg(manaSymbol));
    QImage manaImgScaled = manaImg.scaled(manaSize, manaSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(manaX, manaY, manaImgScaled);
}

bool DeckTrackerUI::isMouseOver(QMouseEvent *event)
{
    QRect uiRect = QRect(pos.x(), pos.y(), uiWidth, uiHeight);
    return uiRect.contains(event->pos());
}

void DeckTrackerUI::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mousePressed = true;
        mouseRelativePosition = event->pos() - pos;
    }
}

void DeckTrackerUI::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        pos = event->pos() - mouseRelativePosition;
    }
}

void DeckTrackerUI::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mousePressed = false;
        mouseRelativePosition = QPoint();
    }
}
