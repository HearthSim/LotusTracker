#include "decktrackerbase.h"
#include "ui_decktrackerbase.h"
#include "../macros.h"

#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QUrlQuery>
#include <tuple>

#ifdef Q_OS_MAC
#include <objc/objc-runtime.h>
#endif

#define GATHERER_IMAGE_URL "http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=%1&type=card"

DeckTrackerBase::DeckTrackerBase(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::DeckTracker()), cardBGSkin(APP_SETTINGS->getCardLayout()),
    zoomMinusButton(QRect(0, 0, 0, 0)), zoomPlusButton(QRect(0, 0, 0, 0)),
    currentHoverPosition(0), hoverCardMultiverseId(0), mousePressed(false),
    mouseRelativePosition(QPoint()), cornerRadius(10), uiPos(10, 10),
    uiAlpha(1.0), uiScale(1.0), cardHoverWidth(200), uiHeight(0),
    uiWidth(160), deck(Deck()), hidden(false)
{
    ui->setupUi(this);
    setupWindow();
    setupDrawTools();
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    cachesDir = dataDir + QDir::separator() + "caches";
    if (!QFile::exists(cachesDir)) {
        QDir dir;
        dir.mkpath(cachesDir);
    }
    changeAlpha(APP_SETTINGS->getDeckTrackerAlpha());
    unhiddenTimeout = APP_SETTINGS->getUnhiddenDelay();
    showCardOnHover = APP_SETTINGS->isShowCardOnHoverEnabled();

    unhiddenTimer = new QTimer();
    connect(unhiddenTimer, &QTimer::timeout, this, [this]{
        hidden = false;
        update();
    });
}

DeckTrackerBase::~DeckTrackerBase()
{
    delete ui;
    DEL(unhiddenTimer)
            for (CardBlinkInfo *cardBlinkInfo : cardsBlinkInfo.values()){
        delete cardBlinkInfo;
    }
}

Deck DeckTrackerBase::getDeck()
{
    return deck;
}

// Credits to Track o'bot - https://github.com/stevschmid/track-o-bot
void DeckTrackerBase::setupWindow()
{
    setWindowFlags(Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_Hover, true);
#ifdef Q_OS_MAC
    setWindowFlags(windowFlags() | Qt::Window);
    WId windowObject = this->winId();
    objc_object* nsviewObject = reinterpret_cast<objc_object*>(windowObject);
    objc_object* nsWindowObject = objc_msgSend(nsviewObject, sel_registerName("window"));
    int NSWindowCollectionBehaviorCanJoinAllSpaces = 1 << 0;
    objc_msgSend(nsWindowObject, sel_registerName("setCollectionBehavior:"), NSWindowCollectionBehaviorCanJoinAllSpaces);
    objc_msgSend(nsWindowObject, sel_registerName("setLevel:"), sel_registerName("NSFloatingWindowLevel"));
#else
    setWindowFlags(windowFlags() | Qt::Tool);
#endif

    QRect screen = QApplication::desktop()->screenGeometry();
    move(0, 0);
    resize(screen.width(), screen.height() * 1.2);
    show();
    hide();
}

void DeckTrackerBase::setupDrawTools()
{
    bgPen = QPen(QColor(160, 160, 160));
    bgPen.setWidth(1);
#if defined Q_OS_MAC
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
#else
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/OpenSans-Regular.ttf");
#endif
    // Card
    int cardFontSize = 7;
#if defined Q_OS_MAC
    cardFontSize += 2;
#endif
    cardFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    cardFont.setPointSize(cardFontSize);
    cardFont.setBold(false);
    cardPen = QPen(Qt::black);
    cardNonePen = QPen(QColor(80, 80, 80));
    // Title
    int titleFontSize = 11;
#if defined Q_OS_MAC
    titleFontSize += 4;
#endif
    titleFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    titleFont.setPointSize(titleFontSize);
    titleFont.setBold(true);
    titlePen = QPen(Qt::white);
}

int DeckTrackerBase::getCardHeight()
{
    int stackCardsPixels = 1;
    return (uiWidth/7) - stackCardsPixels;
}

QList<Card*> DeckTrackerBase::getDeckCardsSorted()
{
    QList<Card*> sortedDeckCards(deck.currentCards().keys());
    std::sort(std::begin(sortedDeckCards), std::end(sortedDeckCards), [](Card*& lhs, Card*& rhs) {
        return std::make_tuple(lhs->isLand, lhs->manaCostValue(), lhs->name) <
                std::make_tuple(rhs->isLand, rhs->manaCostValue(), rhs->name);
    });
    return sortedDeckCards;
}

void DeckTrackerBase::changeAlpha(int alpha)
{
    uiAlpha = 0.3 + (alpha / 10.0);
    update();
}

void DeckTrackerBase::changeCardLayout(QString cardLayout)
{
    cardBGSkin = cardLayout;
    update();
}

void DeckTrackerBase::onShowCardOnHoverEnabled(bool enabled)
{
    showCardOnHover = enabled;
    update();
}

void DeckTrackerBase::changeUnhiddenTimeout(int timeout)
{
    unhiddenTimeout = timeout;
    update();
}

void DeckTrackerBase::blinkCard(Card* card)
{
    QTimer *blinkTimer = new QTimer();
    CardBlinkInfo *cardBlinkInfo = new CardBlinkInfo(this, card, blinkTimer);
    cardsBlinkInfo[card] = cardBlinkInfo;
    connect(blinkTimer, &QTimer::timeout, cardBlinkInfo, &CardBlinkInfo::timeout);
    blinkTimer->start(100);
}

void DeckTrackerBase::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setOpacity(uiAlpha);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.save();
    painter.scale(uiScale, uiScale);
    drawCover(painter);
    drawCoverButtons(painter);
    drawDeckInfo(painter);
    if (hidden) {
        drawExpandBar(painter);
    } else {
        drawDeckCards(painter);
    }
    afterPaintEvent(painter);
    drawHoverCard(painter);
    painter.restore();
}

void DeckTrackerBase::drawCover(QPainter &painter)
{
    // Cover BG
    QRect coverRect(uiPos.x(), uiPos.y(), uiWidth, uiWidth/4);
    painter.setPen(bgPen);
    painter.drawRoundedRect(coverRect, cornerRadius, cornerRadius);
    // Cover image
    bool coverImgLoaded = false;
    QImage coverImg;
    QString deckColorIdentity = onGetDeckColorIdentity();
    coverImgLoaded = coverImg.load(QString(":/res/covers/%1.jpg").arg(deckColorIdentity));
    if (!coverImgLoaded) {
        coverImg.load(":/res/covers/default.jpg");
    }
    QSize coverImgSize(coverRect.width() - 1, coverRect.height() - 1);
    QImage coverImgScaled = coverImg.scaled(coverImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage coverImgWithRoundedCorders = Transformations::applyRoundedCorners2Image(coverImgScaled, cornerRadius);
    painter.drawImage(uiPos.x() + 1, uiPos.y() + 1, coverImgWithRoundedCorders);
    uiHeight = coverRect.height();
}

void DeckTrackerBase::drawCoverButtons(QPainter &painter)
{
    int zoomButtonSize = 12;
    int zoomButtonMargin = 4;
    int zoomButtonY = uiPos.y() + uiHeight - zoomButtonSize - zoomButtonMargin;
    // Plus button
    QImage zoomPlus(":res/zoom_plus.png");
    QImage zoomPlusScaled = zoomPlus.scaled(zoomButtonSize, zoomButtonSize,
                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int zoomPlusX = uiPos.x() + uiWidth - zoomButtonSize - zoomButtonMargin;
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

void DeckTrackerBase::drawDeckInfo(QPainter &painter)
{
    // Deck title
    QFontMetrics titleMetrics(titleFont);
    int titleHeight = titleMetrics.ascent() - titleMetrics.descent();
    int titleTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    drawText(painter, titleFont, titlePen, deck.name, titleTextOptions, true,
             uiPos.x() + 8, uiPos.y() + 5, titleHeight, uiWidth);
    // Deck identity
    int manaSize = 12;
    int manaX = uiPos.x() + 8;
    int manaY = uiPos.y() + uiHeight - manaSize - 4;
    QString deckColorIdentity = onGetDeckColorIdentity();
    if (deckColorIdentity != "m"){
        for (int i=0; i<deckColorIdentity.length(); i++) {
            QChar manaSymbol = deckColorIdentity.at(i);
            drawMana(painter, manaSymbol, manaSize, false, manaX, manaY);
            manaX += manaSize + 5;
        }
    }
}

void DeckTrackerBase::drawDeckCards(QPainter &painter)
{
    int cardListHeight = 0;
    QSize cardBGImgSize(uiWidth, uiWidth/7);
    QFontMetrics cardMetrics(cardFont);
    int cardTextHeight = cardMetrics.ascent() - cardMetrics.descent();
    int cardTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    int cardQtdOptions = Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip;
    for (Card* card : getDeckCardsSorted()) {
        int cardQtdRemains = deck.currentCards()[card];
        // Card BG
        int cardBGY = uiPos.y() + uiHeight + cardListHeight;
        QImage cardBGImg;
        QString borderColorIdentity = card->borderColorIdentityAsString();
        cardBGImg.load(QString(":/res/cards/%1/%2.png").arg(cardBGSkin).arg(borderColorIdentity));
        QImage cardBGImgScaled = cardBGImg.scaled(cardBGImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (cardQtdRemains == 0) {
            cardBGImgScaled = Transformations::toGrayscale(cardBGImgScaled);
        }
        painter.drawImage(uiPos.x(), cardBGY, cardBGImgScaled);
        QPen cardTextPen = cardQtdRemains == 0 ? cardNonePen : cardPen;
        // Card quantity
        painter.setFont(cardFont);
        QString cardQtd = QString("%1 ").arg(cardQtdRemains);
        int cardQtdMargin = 12;
        int cardQtdWidth = painter.fontMetrics().width(cardQtd);
        int cardQtdX = uiPos.x() + cardQtdMargin;
        int cardQtdY = cardBGY + cardBGImgSize.height()/2 - cardTextHeight/2;
#if defined Q_OS_WIN
        cardQtdY -= 1;
#endif
        drawText(painter, cardFont, cardTextPen, cardQtd, cardQtdOptions, false,
                 cardQtdX, cardQtdY, cardTextHeight, cardQtdWidth);
        // Card mana
        int manaRightMargin = 7;
        int manaMargin = 2;
        int manaSize = 8;
        int manaCostWidth = card->manaCost.length() * (manaSize + manaMargin);
        int manaX = uiPos.x() + uiWidth - manaRightMargin - manaCostWidth;
        int manaY = cardBGY + cardBGImgSize.height()/2 - manaSize/2;
        for (QChar manaSymbol : card->manaCost) {;
            drawMana(painter, manaSymbol, manaSize, cardQtdRemains == 0, manaX, manaY);
            manaX += manaSize + manaMargin;
        }
        // Card name
        int manaWidth = manaCostWidth + manaRightMargin;
        int cardNameWidth = uiWidth - cardQtdMargin - cardQtdWidth - manaWidth;
        QFontMetrics metrics(cardFont);
        QString cardName = metrics.elidedText(card->name, Qt::ElideRight, cardNameWidth);
        drawText(painter, cardFont, cardTextPen, cardName, cardTextOptions, false,
                 cardQtdX + cardQtdWidth, cardQtdY, cardTextHeight, cardNameWidth);
        cardListHeight += getCardHeight();
        // Blink
        if (cardsBlinkInfo.keys().contains(card)) {
            CardBlinkInfo *cardBlinkInfo = cardsBlinkInfo[card];
            if (cardBlinkInfo->alpha > 0) {
                QRect coverRect(uiPos.x(), cardBGY, cardBGImgSize.width(), cardBGImgSize.height());
                painter.setPen(QPen(QColor(255, 255, 255)));
                painter.setBrush(QBrush(QColor(255, 255, 155, cardBlinkInfo->alpha)));
                painter.drawRoundedRect(coverRect, cornerRadius, cornerRadius);
            } else {
                cardsBlinkInfo.remove(cardBlinkInfo->card);
                delete cardBlinkInfo;
            }
        }
    }
    cardsRect = QRect(uiPos.x(), uiHeight, uiWidth, uiHeight + cardListHeight);
    uiHeight += cardListHeight;
}

void DeckTrackerBase::drawExpandBar(QPainter &painter)
{
    // Expand BG
    int expandCornerRadius = 4;
    QRect expandRect(uiPos.x(), uiPos.y() + uiHeight, uiWidth, 12);
    painter.setPen(bgPen);
    painter.setBrush(QBrush(QColor(70, 70, 70, 175)));
    painter.drawRoundedRect(expandRect, expandCornerRadius, expandCornerRadius);

    int expandButtonSize = 14;
    int expandButtonY = uiPos.y() + uiHeight;
    // Plus button
    QImage expandPlus(":res/expand.png");
    QImage expandPlusScaled = expandPlus.scaled(expandButtonSize, expandButtonSize,
                                                Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int expandPlusX = uiPos.x() + uiWidth/2 - expandButtonSize/2;
    painter.drawImage(expandPlusX, expandButtonY, expandPlusScaled);
    expandBar = expandRect;
}

void DeckTrackerBase::drawHoverCard(QPainter &painter)
{
    if (!showCardOnHover || hoverCardMultiverseId == 0) {
        return;
    }
    int bottomMargin = 10;
    int cardHoverHeight = cardHoverWidth / 0.7;
    int cardHoverMargin = 10;
    QSize cardHoverSize(cardHoverWidth, cardHoverHeight);
    QImage cardImg(":res/cardback.png");
    QString imageFile = QString("%1%2%3.png").arg(cachesDir)
            .arg(QDir::separator()).arg(hoverCardMultiverseId);
    if (QFile::exists(imageFile)) {
        cardImg.load(imageFile);
    } else {
        QUrl url(QString(GATHERER_IMAGE_URL).arg(hoverCardMultiverseId));
        QNetworkRequest request(url);
        QNetworkReply *reply = networkManager.get(request);
        connect(reply, &QNetworkReply::finished,
                this, &DeckTrackerBase::onCardImageDownloaded);
    }
    QRect screen = QApplication::desktop()->screenGeometry();
    int cardX = uiPos.x() - cardHoverWidth - cardHoverMargin;
    if (cardX < cardHoverMargin) {
        cardX = uiPos.x() + uiWidth + cardHoverMargin;
    }
    int cardY = uiPos.y() + (currentHoverPosition * getCardHeight());
    if (cardY > screen.height() - cardHoverHeight - bottomMargin) {
        cardY = screen.height() - cardHoverHeight - bottomMargin;
    }
    QImage cardImgScaled = cardImg.scaled(cardHoverSize,
                                          Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.drawImage(cardX, cardY, cardImgScaled);
}

void DeckTrackerBase::onCardImageDownloaded()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QImage cardImg;
    cardImg.loadFromData(reply->readAll());
    QString query = reply->request().url().query();
    QString multiverseId = QUrlQuery(query).queryItemValue("multiverseid");
    cardImg.save(QString("%1%2%3.png").arg(cachesDir)
                 .arg(QDir::separator()).arg(multiverseId));
    update();
}

void DeckTrackerBase::drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
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

void DeckTrackerBase::drawMana(QPainter &painter, QChar manaSymbol, int manaSize,
                               bool grayscale, int manaX, int manaY)
{
    QImage manaImg;
    manaImg.load(QString(":/res/mana/%1.png").arg(manaSymbol));
    QImage manaImgScaled = manaImg.scaled(manaSize, manaSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (grayscale) {
        painter.drawImage(manaX, manaY, Transformations::toGrayscale(manaImgScaled));
    } else {
        painter.drawImage(manaX, manaY, manaImgScaled);
    }
}

bool DeckTrackerBase::event(QEvent *event)
{
    switch(event->type()){
    case QEvent::HoverEnter:
        onHoverEnter(static_cast<QHoverEvent*>(event));
        return true;
        break;
    case QEvent::HoverMove:
        onHoverMove(static_cast<QHoverEvent*>(event));
        return true;
        break;
    case QEvent::HoverLeave:
        onHoverLeave(static_cast<QHoverEvent*>(event));
        return true;
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void DeckTrackerBase::onHoverEnter(QHoverEvent *event)
{
    updateCardHoverUrl(getCardsHoverPosition(event));
}

void DeckTrackerBase::onHoverMove(QHoverEvent *event)
{
    int hoverPosition = getCardsHoverPosition(event);
    if (hoverPosition != currentHoverPosition) {
        updateCardHoverUrl(hoverPosition);
    }
}

void DeckTrackerBase::onHoverLeave(QHoverEvent *event)
{
    UNUSED(event);
    hoverCardMultiverseId = 0;
    currentHoverPosition = -1;
    update();
}

int DeckTrackerBase::getCardsHoverPosition(QHoverEvent *event)
{
    int cardsHoverY = event->pos().y() - cardsRect.y() - uiPos.y();
    if (cardsHoverY < 0) {
        return -1;
    } else {
        return cardsHoverY / getCardHeight();
    }
}

void DeckTrackerBase::updateCardHoverUrl(int hoverPosition)
{
    if (hoverPosition < 0 || hoverPosition >= deck.currentCards().size()){
        return;
    }
    currentHoverPosition = hoverPosition;
    Card* cardHovered = getDeckCardsSorted()[currentHoverPosition];
    hoverCardMultiverseId = cardHovered->multiverseId;
    LOGD(QString("%1 - %2").arg(currentHoverPosition).arg(cardHovered->name));
    update();
}

void DeckTrackerBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (zoomMinusButton.contains(event->pos()) ||
            zoomPlusButton.contains(event->pos())) {
        return;
    }
    mousePressed = true;
    mouseRelativePosition = event->pos() - uiPos;
}

void DeckTrackerBase::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        uiPos = mapToParent(event->pos() - mouseRelativePosition);
        update();
    }
}

void DeckTrackerBase::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        return;
    }
    if (event->button() == Qt::RightButton) {
        onRightClick();
        return;
    }
    if (zoomMinusButton.contains(event->pos())) {
        onZoomMinusClick();
        return;
    }
    if (zoomPlusButton.contains(event->pos())) {
        onZoomPlusClick();
        return;
    }
    if (hidden && expandBar.contains(event->pos())) {
        onExpandBarClick();
        return;
    }
    if (mousePressed) {
        onPositionChanged();
        mousePressed = false;
        mouseRelativePosition = QPoint();
    }
}

void DeckTrackerBase::onRightClick()
{
    hidden = !hidden;
    if (hidden && unhiddenTimeout > 0) {
        unhiddenTimer->start(unhiddenTimeout * 1000);
    } else {
        unhiddenTimer->stop();
    }
    update();
}

void DeckTrackerBase::onExpandBarClick()
{
    hidden = false;
    unhiddenTimer->stop();
    update();
}

void DeckTrackerBase::onZoomMinusClick()
{
    if (uiScale > 0.9) {
        uiScale -= 0.05;
        uiPos += QPoint(uiPos.x()*0.05, 0);
        update();
        onScaleChanged();
    }
}

void DeckTrackerBase::onZoomPlusClick()
{
    if (uiScale < 1.1) {
        uiScale += 0.05;
        uiPos -= QPoint(uiPos.x()*0.05, 0);
        update();
        onScaleChanged();
    }
}
