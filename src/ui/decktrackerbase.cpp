#include "decktrackerbase.h"
#include "ui_decktrackerbase.h"
#include "../macros.h"

#include <QDesktopWidget>
#include <QFontDatabase>
#include <tuple>

#ifdef Q_OS_MAC
#include <objc/objc-runtime.h>
#endif

DeckTrackerBase::DeckTrackerBase(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::DeckTracker()), cardBGSkin(APP_SETTINGS->getCardLayout()),
    zoomMinusButton(QRect(0, 0, 0, 0)), zoomPlusButton(QRect(0, 0, 0, 0)),
    mousePressed(false), mouseRelativePosition(QPoint()), cornerRadius(10),
    uiPos(10, 10), uiScale(1.0), uiHeight(0), uiWidth(160), deck(Deck())
{
    ui->setupUi(this);
    setupWindow();
    setupDrawTools();
}

DeckTrackerBase::~DeckTrackerBase()
{
    delete ui;
    for (CardBlinkInfo *cardBlinkInfo : cardsBlinkInfo.values()){
        delete cardBlinkInfo;
    }
}

void DeckTrackerBase::setupWindow()
{
    setWindowFlags(Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
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

void DeckTrackerBase::changeCardLayout(QString cardLayout)
{
    cardBGSkin = cardLayout;
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
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform);
    painter.save();
    painter.scale(uiScale, uiScale);
    drawCover(painter);
    drawZoomButtons(painter);
    drawDeckInfo(painter);
    drawDeckCards(painter);
    afterPaintEvent(painter);
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
    QString deckColorIdentity = deck.colorIdentity();
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

void DeckTrackerBase::drawZoomButtons(QPainter &painter)
{
    int zoomButtonSize = 12;
    int zoomButtonMargin = 6;
    int zoomButtonY = uiPos.y() + zoomButtonMargin;
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
             uiPos.x() + 8, uiPos.y() + 8, titleHeight, uiWidth);
    // Deck identity
    int manaSize = 12;
    int manaX = uiPos.x() + 8;
    int manaY = uiPos.y() + uiHeight - manaSize - 5;
    QString deckColorIdentity = deck.colorIdentity();
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
    QList<Card*> deckCards(deck.cards.keys());
    std::sort(std::begin(deckCards), std::end(deckCards), [](Card*& lhs, Card*& rhs) {
        return std::make_tuple(lhs->isLand, lhs->manaCostValue(), lhs->name) <
                std::make_tuple(rhs->isLand, rhs->manaCostValue(), rhs->name);
    });
    for (Card* card : deckCards) {
        int cardQtdRemains = deck.cards[card];
        QString cardManaIdentity = card->manaColorIdentityAsString();
        if (cardManaIdentity.size() > 2) {
            cardManaIdentity = "m";
        }
        // Card BG
        int cardBGY = uiPos.y() + uiHeight + cardListHeight;
        QImage cardBGImg;
        cardBGImg.load(QString(":/res/cards/%1/%2.png").arg(cardBGSkin).arg(cardManaIdentity));
        QImage cardBGImgScaled = cardBGImg.scaled(cardBGImgSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (cardQtdRemains == 0) {
            cardBGImgScaled = Transformations::toGrayscale(cardBGImgScaled);
        }
        painter.drawImage(uiPos.x(), cardBGY, cardBGImgScaled);
        QPen cardTextPen = cardQtdRemains == 0 ? cardNonePen : cardPen;
        // Card quantity
        painter.setFont(cardFont);
        QString cardQtd = QString("%1 ").arg(cardQtdRemains);
        int cardQtdWidth = painter.fontMetrics().width(cardQtd);
        int cardQtdX = uiPos.x() + 12;
        int cardQtdY = cardBGY + cardBGImgSize.height()/2 - cardTextHeight/2;
#if defined Q_OS_WIN
        cardQtdY -= 1;
#endif
        drawText(painter, cardFont, cardTextPen, cardQtd, cardQtdOptions, false,
                 cardQtdX, cardQtdY, cardTextHeight, cardQtdWidth);
        // Card name
        drawText(painter, cardFont, cardTextPen, card->name, cardTextOptions, false,
                 cardQtdX + cardQtdWidth, cardQtdY, cardTextHeight, uiWidth - cardQtdWidth);
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
        cardListHeight += cardBGImgSize.height();
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
    uiHeight += cardListHeight;
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
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (zoomMinusButton.contains(event->pos())) {
        if (uiScale > 0.9) {
            uiScale -= 0.05;
            uiPos += QPoint(uiPos.x()*0.05, 0);
            update();
            onScaleChanged();
        }
        return;
    }
    if (zoomPlusButton.contains(event->pos())) {
        if (uiScale < 1.1) {
            uiScale += 0.05;
            uiPos -= QPoint(uiPos.x()*0.05, 0);
            update();
            onScaleChanged();
        }
        return;
    }
    if (mousePressed) {
        onPositionChanged();
        mousePressed = false;
        mouseRelativePosition = QPoint();
    }
}

