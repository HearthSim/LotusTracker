#ifndef DECKTRACKERUI_H
#define DECKTRACKERUI_H

#include "../entity/card.h"
#include "../entity/deck.h"
#include "blinkinfo.h"

#include <QObject>
#include <QPainter>
#include <QPoint>
#include <QMainWindow>
#include <QMouseEvent>

class DeckTrackerUI : public QObject
{
    Q_OBJECT
private:
    QMainWindow *parentQMainWindow;
    QPoint pos;
    int uiHeight;
    int uiWidth;
    QString cardBGSkin;
    Deck deck;
    bool deckLoaded;
    // Draw
    QPen bgPen;
    QBrush bgBrush;
    QFont cardFont;
    QPen cardPen;
    QFont titleFont;
    QPen titlePen;
    QFont statisticsFont;
    QPen statisticsPen;
    QMap<Card*, BlinkInfo*> cardsBlink;
    bool mousePressed;
    QPoint mouseRelativePosition;
    void blinkCard(Card* card);
    void drawCover(QPainter &painter);
    void drawDeckInfo(QPainter &painter);
    void drawDeckCards(QPainter &painter);
    void drawStatistics(QPainter &painter);
    void drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
                  bool shadow, int textX, int textY, int textHeight, int textWidth);
    void drawMana(QPainter &painter, QChar manaSymbol, int manaSize,
                  bool grayscale, int manaX, int manaY);

public:
    explicit DeckTrackerUI(QMainWindow *parent = nullptr);
    ~DeckTrackerUI();
    int getWidth();
    void move(int x, int y);
    void setupDeck(Deck deck);
    void drawCard(Card* card, bool opponent);
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
