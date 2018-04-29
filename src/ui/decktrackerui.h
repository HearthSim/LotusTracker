#ifndef DECKTRACKERUI_H
#define DECKTRACKERUI_H

#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>

class DeckTrackerUI : public QObject
{
    Q_OBJECT
private:
    QPoint pos;
    int _width, _height;
    QPen coverPen;
    QBrush coverBrush;
    bool mousePressed;
    QPoint mousePosition;

public:
    explicit DeckTrackerUI(QObject *parent = nullptr);
    void move(int x, int y);
    int height();
    int width();
    void paintEvent(QPainter &painter);
    bool isMouseOver(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:
};

#endif // DECKTRACKERUI_H
