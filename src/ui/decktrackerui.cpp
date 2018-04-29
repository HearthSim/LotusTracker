#include "decktrackerui.h"
#include "../macros.h"

DeckTrackerUI::DeckTrackerUI(QObject *parent) : QObject(parent),
    _width(150), _height(65), mousePressed(false), mousePosition(QPoint())
{
    move(10, 10);
    coverPen = QPen(QColor(160, 160, 160));
    coverPen.setWidth(3);
    coverBrush = QBrush(QColor(70, 70, 70, 175));
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

void DeckTrackerUI::paintEvent(QPainter &painter)
{
    QRect coverRect(pos.x(), pos.y(), _width, _height);
    painter.setPen(coverPen);
    painter.setBrush(coverBrush);
    painter.setClipRect(coverRect);
    painter.drawRoundedRect(coverRect, 10, 10);
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
        mousePosition = event->pos() - pos;
    }
}

void DeckTrackerUI::mouseMoveEvent(QMouseEvent *event)
{
    if (mousePressed) {
        pos = event->pos() - mousePosition;
    }
}

void DeckTrackerUI::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mousePressed = false;
        mousePosition = QPoint();
    }
}
