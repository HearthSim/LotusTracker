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
    qreal uiScale;
    int uiHeight, uiWidth;
    QRect zoomMinusButton, zoomPlusButton;
    QString cardBGSkin;
    Deck deck;
    bool deckLoaded;
    // Draw
    QPen bgPen, cardPen, titlePen, statisticsPen;
    QBrush bgBrush;
    QFont cardFont, titleFont, statisticsFont;
    QMap<Card*, BlinkInfo*> cardsBlink;
    bool mousePressed;
    QPoint mouseRelativePosition;
    void update();
    void blinkCard(Card* card);
    void drawCover(QPainter &painter);
    void drawZoomButtons(QPainter &painter);
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
