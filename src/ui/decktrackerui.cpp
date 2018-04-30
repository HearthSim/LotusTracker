#include "decktrackerui.h"
#include "../macros.h"

#include <QFontDatabase>

DeckTrackerUI::DeckTrackerUI(QObject *parent) : QObject(parent),
    _width(200), _height(50), deckLoaded(false), mousePressed(false), mouseRelativePosition(QPoint())
{
    move(10, 10);
    coverPen = QPen(QColor(160, 160, 160));
    coverPen.setWidth(3);
    coverBrush = QBrush(QColor(70, 70, 70, 175));
    titlePen = QPen(Qt::white);
    titleShadowPen = QPen(Qt::gray);
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
    titleFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleTextOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
}

DeckTrackerUI::~DeckTrackerUI()
{

}

void DeckTrackerUI::move(int x, int y)
{
    pos = QPoint(x, y);
}

int DeckTrackerUI::height()
{
    return _height;
}

int DeckTrackerUI::width()
{
    return _width;
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
    drawDeckInfo(painter);
}

void DeckTrackerUI::drawCover(QPainter &painter)
{
    // Cover BG
    QRect coverRect(pos.x(), pos.y(), _width, _height);
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
}

void DeckTrackerUI::drawDeckInfo(QPainter &painter)
{
    if (!deckLoaded) {
        return;
    }
    painter.setFont(titleFont);
    QFontMetrics titleMetrics(titleFont);
    int titleHeight = titleMetrics.ascent() - titleMetrics.descent();
    int x = pos.x() + 10;
    int y = pos.y() + 10;
    // Title  shadow
    painter.setPen(titleShadowPen);
    painter.drawText(x - 1, y + 1, _width, titleHeight, titleTextOptions, deck.name);
    // Deck title
    painter.setPen(titlePen);
    painter.drawText(x, y, _width, titleHeight, titleTextOptions, deck.name);
}

bool DeckTrackerUI::isMouseOver(QMouseEvent *event)
{
    QRect uiRect = QRect(pos.x(), pos.y(), _width, _height);
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
