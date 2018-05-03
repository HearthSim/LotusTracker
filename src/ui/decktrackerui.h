#ifndef DECKTRACKERUI_H
#define DECKTRACKERUI_H

#include "../entity/deck.h"

#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>

class DeckTrackerUI : public QObject
{
    Q_OBJECT
private:
    QPoint pos;
    int height;
    int width;
    Deck deck;
    bool deckLoaded;
    QPen coverPen;
    QBrush coverBrush;
    QPen titlePen;
    QPen titleShadowPen;
    QFont titleFont;
    int titleTextOptions;
    bool mousePressed;
    QPoint mouseRelativePosition;
    int drawCover(QPainter &painter);
    void drawDeckInfo(QPainter &painter);

public:
    explicit DeckTrackerUI(QObject *parent = nullptr);
    ~DeckTrackerUI();
    int getWidth();
    void move(int x, int y);
    void setupDeck(Deck deck);
    void paintEvent(QPainter &painter);
    // Dragging functions
    bool isMouseOver(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:
};

#endif // DECKTRACKERUI_H
