#include "deckoverlaybase.h"
#include "ui_decktrackerbase.h"
#include "../macros.h"

#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QToolTip>
#include <QUrlQuery>
#include <tuple>
#include <QGraphicsScene>

#ifdef Q_OS_MAC
#include <objc/objc-runtime.h>
#endif

#define BASE_UI_WIDTH 160

DeckOverlayBase::DeckOverlayBase(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::TrackerOverlay()), cardBGSkin(APP_SETTINGS->getCardLayout()),
    currentHoverPosition(0), hoverCard(nullptr), mousePressed(false),
    mouseInitialPosition(QPoint()), cornerRadius(10), uiPos(0, 50),
    zoomMinusButton(QRect(0, 0, 0, 0)), zoomPlusButton(QRect(0, 0, 0, 0)),
    uiAlpha(1.0), cardHoverWidth(220), uiHeight(0), uiWidth(BASE_UI_WIDTH),
    uiScale(2), deck(Deck()), hidden(false), showingTooltip(false)
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
    stackCardsPixels = 1;
    isShowCardOnHoverEnabled = APP_SETTINGS->isShowCardOnHoverEnabled();
    isShowCardManaCostEnabled = APP_SETTINGS->isShowCardManaCostEnabled();

    unhiddenTimer = new QTimer();
    connect(unhiddenTimer, &QTimer::timeout, this, [this]{
        hidden = false;
        update();
    });
}

DeckOverlayBase::~DeckOverlayBase()
{
    delete ui;
    DEL(unhiddenTimer)
            for (CardBlinkInfo *cardBlinkInfo : cardsBlinkInfo.values()){
        delete cardBlinkInfo;
    }
}

Deck DeckOverlayBase::getDeck()
{
    return deck;
}

// Credits to Track o'bot - https://github.com/stevschmid/track-o-bot
void DeckOverlayBase::setupWindow()
{
    setWindowFlags(Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowTitle(TITLE());
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
    screen = QApplication::desktop()->screenGeometry();
    move(0, 0);
    setFixedSize(screen.width() / 2, QApplication::desktop()->height());
    resize(screen.width(), static_cast<int> (screen.height() * 1.2));
    show();
    hide();
}

void DeckOverlayBase::setupDrawTools()
{
    bgPen = QPen(QColor(160, 160, 160));
    bgPen.setWidth(1);
#if defined Q_OS_MAC
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
#else
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/OpenSans-Regular.ttf");
#endif
    // Card
    cardFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    cardFont.setBold(false);
    cardPen = QPen(Qt::black);
    cardNonePen = QPen(QColor(80, 80, 80));
    // Title
    titleFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    titleFont.setBold(true);
    titlePen = QPen(Qt::white);
    QFontMetrics titleMetrics(titleFont);
    titleHeight = titleMetrics.ascent() - titleMetrics.descent();
    onScaleChanged();
}

int DeckOverlayBase::getCardHeight()
{
    float ratio = isShowCardManaCostEnabled ? 7 : 6.1f;
    return static_cast<int>(uiWidth / ratio) - stackCardsPixels;
}

QList<Card*> DeckOverlayBase::getDeckCardsSorted()
{
    QList<Card*> sortedDeckCards(deck.currentCards().keys());
    std::sort(std::begin(sortedDeckCards), std::end(sortedDeckCards), [](Card*& lhs, Card*& rhs) {
        return std::make_tuple(lhs->isLand, lhs->manaCostValue(), lhs->name) <
                std::make_tuple(rhs->isLand, rhs->manaCostValue(), rhs->name);
    });
    return sortedDeckCards;
}

void DeckOverlayBase::changeAlpha(int alpha)
{
    uiAlpha = 0.3 + (alpha / 10.0);
    update();
}

void DeckOverlayBase::changeCardLayout(QString cardLayout)
{
    cardBGSkin = cardLayout;
    update();
}

void DeckOverlayBase::onShowCardManaCostEnabled(bool enabled)
{
    isShowCardManaCostEnabled = enabled;
    update();
}

void DeckOverlayBase::onShowCardOnHoverEnabled(bool enabled)
{
    isShowCardOnHoverEnabled = enabled;
    update();
}

void DeckOverlayBase::changeUnhiddenTimeout(int timeout)
{
    unhiddenTimeout = timeout;
    update();
}

void DeckOverlayBase::blinkCard(Card* card)
{
    QTimer *blinkTimer = new QTimer();
    CardBlinkInfo *cardBlinkInfo = new CardBlinkInfo(this, card, blinkTimer);
    cardsBlinkInfo[card] = cardBlinkInfo;
    connect(blinkTimer, &QTimer::timeout, cardBlinkInfo, &CardBlinkInfo::timeout);
    blinkTimer->start(100);
}

void DeckOverlayBase::hideCardOnHover()
{
    hoverCard = nullptr;
}

void DeckOverlayBase::paintEvent(QPaintEvent*)
{
    uiWidth = BASE_UI_WIDTH - (isShowCardManaCostEnabled ? 0 : 20) + uiScale * 10;
    QPainter painter(this);
    painter.setOpacity(uiAlpha);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.save();
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

void DeckOverlayBase::drawCover(QPainter &painter)
{
    // Cover BG
    float ratio = isShowCardManaCostEnabled ? 4 : 3.5f;
    int height = static_cast<int>(uiWidth/ratio);
    QRect coverRect(uiPos.x(), uiPos.y(), uiWidth, height);
    painter.setPen(bgPen);
    painter.drawRoundedRect(coverRect, cornerRadius, cornerRadius);
    // Cover image
    bool coverImgLoaded = false;
    QImage coverImg;
    QString deckColorIdentity = getDeckColorIdentity();
    coverImgLoaded = coverImg.load(QString(":/res/covers/%1.jpg").arg(deckColorIdentity));
    if (!coverImgLoaded) {
        coverImg.load(":/res/covers/default.jpg");
    }
    QSize coverImgSize(coverRect.width() - 1, coverRect.height() - 1);
    QImage coverImgScaled = coverImg.scaled(coverImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage coverImgWithRoundedCorders = Transformations::applyRoundedCorners2Image(coverImgScaled, cornerRadius);
    painter.drawImage(uiPos.x() + 1, uiPos.y() + 1, coverImgWithRoundedCorders);
    // Img Opacity
    QRect coverImgRect(uiPos.x() + 1, uiPos.y() + 1, uiWidth, height);
    painter.setBrush(QBrush(QColor(70, 70, 70, 50)));
    painter.drawRoundedRect(coverImgRect, cornerRadius, cornerRadius);
    uiHeight = coverImgRect.height();
}

void DeckOverlayBase::drawCoverButtons(QPainter &painter)
{
    int zoomButtonSize = 12 + static_cast<int> (uiScale * 1);
    int zoomButtonMargin = 5;
    int zoomButtonY = uiPos.y() + zoomButtonMargin;
    // Minus button
    int zoomMinusX = uiPos.x() + zoomButtonMargin + 2;
    QImage zoomMinus(":res/zoom_minus.png");
    QImage zoomMinusScaled = zoomMinus.scaled(zoomButtonSize, zoomButtonSize,
                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(zoomMinusX, zoomButtonY, zoomMinusScaled);
    zoomMinusButton = QRect(zoomMinusX, zoomButtonY, zoomButtonSize, zoomButtonSize);
    // Plus button
    int zoomPlusX = static_cast<int> (zoomMinusX + zoomButtonSize + zoomButtonMargin);
    QImage zoomPlus(":res/zoom_plus.png");
    QImage zoomPlusScaled = zoomPlus.scaled(zoomButtonSize, zoomButtonSize,
                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.drawImage(zoomPlusX, zoomButtonY, zoomPlusScaled);
    zoomPlusButton = QRect(zoomPlusX, zoomButtonY, zoomButtonSize, zoomButtonSize);
}

void DeckOverlayBase::drawDeckInfo(QPainter &painter)
{
    // Deck title
    QFontMetrics titleMetrics(titleFont);
    int titleTextOptions = Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextDontClip;
    QString deckName = titleMetrics.elidedText(deck.name, Qt::ElideRight, uiWidth);
    drawText(painter, titleFont, titlePen, deckName, titleTextOptions, true,
             uiPos.x(), getDeckNameYPosition(), titleHeight, uiWidth);
    // Deck identity
    int manaSize = 13 + static_cast<int> (uiScale * 1);
    int manaX = uiPos.x() + 8;
    int manaY = uiPos.y() + uiHeight - manaSize - 4;
    QString deckColorIdentity = getDeckColorIdentity();
    if (deckColorIdentity != "m"){
        for (int i=0; i<deckColorIdentity.length(); i++) {
            QChar manaSymbol = deckColorIdentity.at(i);
            drawMana(painter, manaSymbol, manaSize, false, manaX, manaY);
            manaX += manaSize + 3;
        }
    }
}

void DeckOverlayBase::drawDeckCards(QPainter &painter)
{
    int cardListHeight = 0;
    QSize cardBGImgSize(uiWidth, getCardHeight() + stackCardsPixels);
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
        int manaWidth = manaRightMargin;
        if (isShowCardManaCostEnabled) {
            int manaMargin = 2;
            int manaSize = 8 + static_cast<int> (uiScale * 0.5);
            int manaCostWidth = card->manaSymbols.length() * (manaSize + manaMargin);
            int manaX = uiPos.x() + uiWidth - manaRightMargin - manaCostWidth;
            int manaY = cardBGY + cardBGImgSize.height()/2 - manaSize/2;
            for (QString manaSymbol : card->manaSymbols) {;
                drawMana(painter, manaSymbol, manaSize, cardQtdRemains == 0, manaX, manaY);
                manaX += manaSize + manaMargin;
            }
            manaWidth += manaCostWidth;
        }
        // Card name
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

void DeckOverlayBase::drawExpandBar(QPainter &painter)
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

void DeckOverlayBase::drawHoverCard(QPainter &painter)
{
    if (!isShowCardOnHoverEnabled || hoverCard == nullptr) {
        return;
    }
    int bottomMargin = 10;
    int cardHoverHeight = static_cast<int> (cardHoverWidth / 0.7);
    QSize cardHoverSize(cardHoverWidth, cardHoverHeight);
    QImage cardImg(":res/cardback.png");
    QString imageFile = QString("%1%2%3.png").arg(cachesDir)
            .arg(QDir::separator()).arg(hoverCard->mtgaId);
    if (QFile::exists(imageFile)) {
        cardImg.load(imageFile);
    } else {
        QUrl url(hoverCard->imageUrl);
        QNetworkRequest request(url);
        request.setRawHeader("mtgaid", QString::number(hoverCard->mtgaId).toUtf8());
        QNetworkReply *reply = networkManager.get(request);
        connect(reply, &QNetworkReply::finished,
                this, &DeckOverlayBase::onCardImageDownloaded);
    }
    QRect screen = QApplication::desktop()->screenGeometry();
    int cardX = getHoverCardXPosition();
    int cardY = uiPos.y() + (currentHoverPosition * getCardHeight());
    if (cardY > screen.height() - cardHoverHeight - bottomMargin) {
        cardY = screen.height() - cardHoverHeight - bottomMargin;
    }
    QImage cardImgScaled = cardImg.scaled(cardHoverSize,
                                          Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.setOpacity(1);
    painter.drawImage(cardX, cardY, cardImgScaled);
    painter.setOpacity(uiAlpha);
}

void DeckOverlayBase::onCardImageDownloaded()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QString mtgaId = reply->request().rawHeader("mtgaid");
    QImage cardImg;
    cardImg.loadFromData(reply->readAll());
    cardImg.save(QString("%1%2%3.png").arg(cachesDir)
                 .arg(QDir::separator()).arg(mtgaId));
    update();
}

void DeckOverlayBase::drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
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

void DeckOverlayBase::drawMana(QPainter &painter, QString manaSymbol, int manaSize,
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

void DeckOverlayBase::onScaleChanged()
{
    // Title
    int titleFontSize = 9 + (uiScale / 2);
#if defined Q_OS_MAC
    titleFontSize += 4;
#endif
    titleFont.setPointSize(titleFontSize);
    // Card
    int cardFontSize = 7 + (uiScale / 2);
#if defined Q_OS_MAC
    cardFontSize += 2;
#endif
    cardFont.setPointSize(cardFontSize);
}

bool DeckOverlayBase::event(QEvent *event)
{
    switch(event->type()){
    case QEvent::HoverEnter:
        if (!hidden) {
            onHoverEnter(static_cast<QHoverEvent*>(event));
        }
        return true;
    case QEvent::HoverMove:
        if (!hidden) {
            onHoverMove(static_cast<QHoverEvent*>(event));
        }
        return true;
    case QEvent::HoverLeave:
        onHoverLeave(static_cast<QHoverEvent*>(event));
        return true;
    default:
        break;
    }
    return QWidget::event(event);
}

void DeckOverlayBase::onHoverEnter(QHoverEvent *event)
{
    if (event->pos().x() < uiPos.x() ||
            event->pos().x() > uiPos.x() + uiWidth) {
        return;
    }
    LOGD("Hove enter");
    updateCardHoverUrl(getCardsHoverPosition(event));
}

void DeckOverlayBase::onHoverMove(QHoverEvent *event)
{
    if (event->pos().x() < uiPos.x() ||
            event->pos().x() > uiPos.x() + uiWidth) {
        return;
    }
    int hoverPosition = getCardsHoverPosition(event);
    if (hoverPosition != currentHoverPosition) {
        updateCardHoverUrl(hoverPosition);
    }
    if (zoomMinusButton.contains(event->pos())) {
        showingTooltip = true;
        QToolTip::showText(event->pos(), tr("Decrement zoom level"));
    }
    if (zoomPlusButton.contains(event->pos())) {
        showingTooltip = true;
        QToolTip::showText(event->pos(), tr("Increment zoom level"));
    }
    if (!showingTooltip) {
        QToolTip::hideText();
    }
    showingTooltip = false;
}

void DeckOverlayBase::onHoverLeave(QHoverEvent *event)
{
    UNUSED(event);
    hoverCard = nullptr;
    currentHoverPosition = -1;
    update();
}

int DeckOverlayBase::getCardsHoverPosition(QHoverEvent *event)
{
    int cardsHoverY = event->pos().y() - cardsRect.y() - uiPos.y();
    if (cardsHoverY < 0) {
        return -1;
    } else {
        return static_cast<int> (cardsHoverY / getCardHeight());
    }
}

void DeckOverlayBase::updateCardHoverUrl(int hoverPosition)
{
    if (hoverPosition < 0 || hoverPosition >= deck.currentCards().size()){
        return;
    }
    currentHoverPosition = hoverPosition;
    Card* cardHovered = getDeckCardsSorted()[currentHoverPosition];
    hoverCard = cardHovered;
    update();
}

void DeckOverlayBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (zoomMinusButton.contains(event->pos()) ||
            zoomPlusButton.contains(event->pos())) {
        return;
    }
    mousePressed = true;
    mouseInitialPosition = event->globalPos() - frameGeometry().topLeft();
    event->accept();
}

void DeckOverlayBase::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        move(event->globalPos() - mouseInitialPosition);
        update();
        event->accept();
    }
}

void DeckOverlayBase::mouseReleaseEvent(QMouseEvent *event)
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
        mouseInitialPosition = QPoint();
    }
}

void DeckOverlayBase::onRightClick()
{
    hidden = !hidden;
    if (hidden && unhiddenTimeout > 0) {
        unhiddenTimer->start(unhiddenTimeout * 1000);
    } else {
        unhiddenTimer->stop();
    }
    update();
    LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Collapse/Expand");
}

void DeckOverlayBase::onExpandBarClick()
{
    hidden = false;
    unhiddenTimer->stop();
    update();
    LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Collapse/Expand");
}

void DeckOverlayBase::onZoomMinusClick()
{
    if (uiScale > 1) {
        uiScale -= 1;
        update();
        onScaleChanged();
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Zoom minor");
    }
}

void DeckOverlayBase::onZoomPlusClick()
{
    if (uiScale < 10) {
        uiScale += 1;
        update();
        onScaleChanged();
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Zoom plus");
    }
}
