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
    int uiHeight;
    int uiWidth;
    QString cardBGSkin;
    Deck deck;
    bool deckLoaded;
    // Draw
    QPen coverPen;
    QBrush coverBrush;
    QFont cardFont;
    QPen cardPen;
    QFont titleFont;
    QPen titlePen;
    bool mousePressed;
    QPoint mouseRelativePosition;
    void drawCover(QPainter &painter);
    void drawDeckInfo(QPainter &painter);
    void drawDeckCards(QPainter &painter);
    void drawTextWithShadow(QPainter &painter, QFont textFont, QPen textPen, QString text,
                            int textOptions, int textX, int textY, int textHeight, int textWidth);
    void drawMana(QPainter &painter, QChar manaSymbol, int manaSize, int manaX, int manaY);

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
