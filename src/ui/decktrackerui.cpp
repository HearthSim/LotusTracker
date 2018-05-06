#include "decktrackerui.h"
#include "../macros.h"

#include <QFontDatabase>
#include <tuple>

#define CORNERS_RADIUS 10

DeckTrackerUI::DeckTrackerUI(QMainWindow *parent) : QObject(parent), uiScale(1.0), uiHeight(0),
    uiWidth(160), zoomMinusButton(QRect(0, 0, 0, 0)), zoomPlusButton(QRect(0, 0, 0, 0)),
    cardBGSkin("mtga"), deckLoaded(false), mousePressed(false), mouseRelativePosition(QPoint())
{
    parentQMainWindow = parent;
    move(10, 10);
    bgPen = QPen(QColor(160, 160, 160));
    bgPen.setWidth(2);
    bgBrush = QBrush(QColor(70, 70, 70, 175));
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
    // Card
    int cardFontSize = 7;
#if defined Q_OS_MAC
    cardFontSize += 2;
#endif
    cardFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    cardFont.setPointSize(cardFontSize);
    cardFont.setBold(true);
    cardPen = QPen(Qt::black);
    // Title
    int titleFontSize = 11;
#if defined Q_OS_MAC
    titleFontSize += 4;
#endif
    titleFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    titleFont.setPointSize(titleFontSize);
    titleFont.setBold(true);
    titlePen = QPen(Qt::white);
    // Statistics
    int statisticsFontSize = 8;
#if defined Q_OS_MAC
    statisticsFontSize += 2;
#endif
    statisticsFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    statisticsFont.setPointSize(statisticsFontSize);
    statisticsPen = QPen(Qt::white);
}

DeckTrackerUI::~DeckTrackerUI()
{
    for (BlinkInfo *blinkInfo : cardsBlink.values()){
        delete blinkInfo;
    }
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

void DeckTrackerUI::drawCard(Card* card, bool opponent)
{
    if (opponent) {
        deck.insertCard(card);
        blinkCard(card);
    } else {
        if (deck.drawCard(card)) {
            blinkCard(card);
        }
    }
}

void DeckTrackerUI::update()
{
    parentQMainWindow->update();
}

void DeckTrackerUI::paintEvent(QPainter &painter)
{
    painter.scale(uiScale, uiScale);
    drawCover(painter);
    drawZoomButtons(painter);
    if (deckLoaded) {
        drawDeckInfo(painter);
        drawDeckCards(painter);
        drawStatistics(painter);
    }
}

void DeckTrackerUI::blinkCard(Card* card)
{
    QTimer *blinkTimer = new QTimer();
    BlinkInfo *blinkInfo = new BlinkInfo(parentQMainWindow, card, blinkTimer);
    cardsBlink[card] = blinkInfo;
    connect(blinkTimer, &QTimer::timeout, blinkInfo, &BlinkInfo::timeout);
    blinkTimer->start(100);
}

void DeckTrackerUI::drawCover(QPainter &painter)
{
    // Cover BG
    QRect coverRect(pos.x(), pos.y(), uiWidth, uiWidth/4);
    painter.setPen(bgPen);
    painter.setBrush(bgBrush);
    painter.drawRoundedRect(coverRect, CORNERS_RADIUS, CORNERS_RADIUS);
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
    QSize coverImgSize(coverRect.width() - 2, coverRect.height() - 2);
    QImage coverImgScaled = coverImg.scaled(coverImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage coverImgWithRoundedCorders = Extensions::applyRoundedCorners2Image(coverImgScaled, CORNERS_RADIUS);
    painter.drawImage(pos.x() + 2, pos.y() + 2, coverImgWithRoundedCorders);
    uiHeight = coverRect.height();
}

void DeckTrackerUI::drawZoomButtons(QPainter &painter)
{
    int zoomButtonSize = 12;
    int zoomButtonMargin = 6;
    int zoomButtonY = pos.y() + zoomButtonMargin;
    // Plus button
    QImage zoomPlus(":res/zoom_plus.png");
    QImage zoomPlusScaled = zoomPlus.scaled(zoomButtonSize, zoomButtonSize,
                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int zoomPlusX = pos.x() + uiWidth - zoomButtonSize - zoomButtonMargin;
    painter.drawImage(zoomPlusX, zoomButtonY, zoomPlusScaled);
    zoomPlusButton = QRect(zoomPlusX*uiScale, zoomButtonY*uiScale,
                           zoomButtonSize*uiScale, zoomButtonSize*uiScale);
    // Minus button
    QImage zoomMinus(":res/zoom_minus.png");
    QImage zoomMinusScaled = zoomMinus.scaled(zoomButtonSize, zoomButtonSize,
                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int zoomMinusX = zoomPlusX - zoomButtonSize - 3*uiScale;
    painter.drawImage(zoomMinusX, zoomButtonY, zoomMinusScaled);
    zoomMinusButton = QRect(zoomMinusX*uiScale, zoomButtonY*uiScale,
                            zoomButtonSize*uiScale, zoomButtonSize*uiScale);
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
            drawMana(painter, manaSymbol, manaSize, false, manaX, manaY);
            manaX += manaSize + 5;
        }
    }
}

void DeckTrackerUI::drawDeckCards(QPainter &painter)
{
    int cardListHeight = 0;
    QSize cardBGImgSize(uiWidth, uiWidth/7);
    QFontMetrics cardMetrics(cardFont);
    int cardTextHeight = cardMetrics.ascent() - cardMetrics.descent();
    int cardTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    int cardQtdOptions = Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip;
    QList<Card*> deckCards(deck.cards.keys());
    std::sort(std::begin(deckCards), std::end(deckCards), [](Card*& lhs, Card*& rhs) {
        return std::make_tuple(lhs->isLand, lhs->manaCostValue(), lhs->name) <
                std::make_tuple(rhs->isLand, rhs->manaCostValue(), rhs->name);
    });
    for (Card* card : deckCards) {
        int cardQtdRemains = deck.cards[card];
        QString cardManaIdentity = card->manaColorIdentityAsString();
        // Card BG
        int cardBGY = pos.y() + uiHeight + cardListHeight;
        QImage cardBGImg;
        cardBGImg.load(QString(":/res/cards/%1/%2.png").arg(cardBGSkin).arg(cardManaIdentity));
        QImage cardBGImgScaled = cardBGImg.scaled(cardBGImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (cardQtdRemains == 0) {
            cardBGImgScaled = Extensions::toGrayscale(cardBGImgScaled);
        }
        painter.drawImage(pos.x(), cardBGY, cardBGImgScaled);
        // Card quantity
        QString cardQtd = QString("%1 ").arg(cardQtdRemains);
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
        for (QChar manaSymbol : card->manaCost) {;
            drawMana(painter, manaSymbol, manaSize, cardQtdRemains == 0, manaX, manaY);
            manaX += manaSize + manaMargin;
        }
        cardListHeight += cardBGImgSize.height();
        // Blink
        if (cardsBlink.keys().contains(card)) {
            BlinkInfo *blinkInfo = cardsBlink[card];
            if (blinkInfo->alpha > 0) {
                QRect coverRect(pos.x(), cardBGY, cardBGImgSize.width(), cardBGImgSize.height());
                painter.setPen(QPen(QColor(255, 255, 0)));
                painter.setBrush(QBrush(QColor(255, 255, 0, blinkInfo->alpha)));
                painter.drawRoundedRect(coverRect, CORNERS_RADIUS, CORNERS_RADIUS);
            } else {
                cardsBlink.remove(blinkInfo->card);
                delete blinkInfo;
            }
        }
    }
    uiHeight += cardListHeight;
}

void DeckTrackerUI::drawStatistics(QPainter &painter)
{
    if (deck.cards.size() == 0 || deck.name.isEmpty()) {
        return;
    }
    // Statistics BG
    QRect coverRect(pos.x(), pos.y() + uiHeight, uiWidth, uiWidth/4);
    painter.setPen(bgPen);
    painter.setBrush(bgBrush);
    painter.drawRoundedRect(coverRect, CORNERS_RADIUS, CORNERS_RADIUS);
    // Statistics info
    QFontMetrics statisticsMetrics(statisticsFont);
    int statisticsTextHeight = statisticsMetrics.ascent() - statisticsMetrics.descent();
    int statisticsTextMargin = 5;
    // Statistics title
    int statisticsBorderMargin = 2;
    int statisticsTitleOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    QString statisticsTitle = QString(tr("Draw chances:"));
    int statisticsTitleX = pos.x() + 8;
    int statisticsTitleY = pos.y() + uiHeight + statisticsTextMargin;
    drawText(painter, statisticsFont, statisticsPen, statisticsTitle, statisticsTitleOptions, false,
             statisticsTitleX, statisticsTitleY, statisticsTextHeight, uiWidth - statisticsBorderMargin);
    // Statistics text
    int statisticsTextOptions = Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip;
    int statisticsTextX = pos.x() + statisticsBorderMargin;
    int statisticsText1Y = statisticsTitleY + statisticsTextHeight + statisticsTextMargin + statisticsBorderMargin;
    float totalCards = deck.totalCards();
    float drawChanceLandCards = deck.totalCardsLand() * 100 / totalCards;
    float drawChance1xCards = deck.totalCardsOfQtd(1) > 0 ? 1 * 100 / totalCards : 0;
    float drawChance2xCards = deck.totalCardsOfQtd(2) > 0 ? 2 * 100 / totalCards : 0;
    float drawChance3xCards = deck.totalCardsOfQtd(3) > 0 ? 3 * 100 / totalCards : 0;
    float drawChance4xCards = deck.totalCardsOfQtd(4) > 0 ? 4 * 100 / totalCards : 0;
    QString statisticsText1 = QString("1x: %1%    2x: %2%    3x: %3%")
            .arg(drawChance1xCards, 0, 'g', 2)
            .arg(drawChance2xCards, 0, 'g', 2)
            .arg(drawChance3xCards, 0, 'g', 2);
    drawText(painter, statisticsFont, statisticsPen, statisticsText1, statisticsTextOptions, false,
             statisticsTextX, statisticsText1Y, statisticsTextHeight, uiWidth - statisticsBorderMargin);
    int statisticsText2Y = statisticsText1Y + statisticsTextHeight + statisticsTextMargin;
    QString statisticsText2 = QString("4x: %1%    Land: %2%")
            .arg(drawChance4xCards, 0, 'g', 2)
            .arg(drawChanceLandCards, 0, 'g', 2);
    drawText(painter, statisticsFont, statisticsPen, statisticsText2, statisticsTextOptions, false,
             statisticsTextX, statisticsText2Y, statisticsTextHeight, uiWidth - statisticsBorderMargin);
    uiHeight += coverRect.height();
}

void DeckTrackerUI::drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
                             bool shadow, int textX, int textY, int textHeight, int textWidth)
{
    painter.setFont(textFont);
    if (shadow) {
        painter.setPen(QPen(Qt::black));
        painter.drawText(textX - 1, textY + 1, textWidth, textHeight, textOptions, text);
    }
    painter.setPen(textPen);
    painter.drawText(textX, textY, textWidth, textHeight, textOptions, text);
}

void DeckTrackerUI::drawMana(QPainter &painter, QChar manaSymbol, int manaSize,
                             bool grayscale, int manaX, int manaY)
{
    QImage manaImg;
    manaImg.load(QString(":/res/mana/%1.png").arg(manaSymbol));
    QImage manaImgScaled = manaImg.scaled(manaSize, manaSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (grayscale) {
        painter.drawImage(manaX, manaY, Extensions::toGrayscale(manaImgScaled));
    } else {
        painter.drawImage(manaX, manaY, manaImgScaled);
    }
}

bool DeckTrackerUI::isMouseOver(QMouseEvent *event)
{
    QRect uiRect = QRect(pos.x(), pos.y(), uiWidth, uiHeight);
    return uiRect.contains(event->pos());
}

void DeckTrackerUI::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (zoomMinusButton.contains(event->pos()) ||
            zoomPlusButton.contains(event->pos())) {
        return;
    }
    mousePressed = true;
    mouseRelativePosition = event->pos() - pos;
}

void DeckTrackerUI::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        pos = event->pos() - mouseRelativePosition;
    }
}

void DeckTrackerUI::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (zoomMinusButton.contains(event->pos())) {
        if (uiScale > 0.9) {
            uiScale -= 0.05;
            update();
        }
    } else if (zoomPlusButton.contains(event->pos())) {
        if (uiScale < 1.1) {
            uiScale += 0.05;
            update();
        }
    } else {
        mousePressed = false;
        mouseRelativePosition = QPoint();
    }
}
