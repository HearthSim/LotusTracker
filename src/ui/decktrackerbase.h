#ifndef DECKTRACKERUI_H
#define DECKTRACKERUI_H

#include "../entity/card.h"
#include "../entity/deck.h"
#include "cardblinkinfo.h"

#include <QMainWindow>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>

namespace Ui { class DeckTracker; }

class DeckTrackerBase : public QMainWindow
{
    Q_OBJECT
private:
    Ui::DeckTracker *ui;
    qreal uiScale;
    QString cardBGSkin;
    QRect zoomMinusButton, zoomPlusButton;
    bool mousePressed;
    QPoint mouseRelativePosition;
    void setupWindow();
    // Draw fields and methods
    QMap<Card*, CardBlinkInfo*> cardsBlinkInfo;
    void setupDrawTools();
    void drawCover(QPainter &painter);
    void drawZoomButtons(QPainter &painter);
    void drawDeckInfo(QPainter &painter);
    void drawDeckCards(QPainter &painter);

protected:
    const int cornerRadius;
    QPoint uiPos;
    int uiHeight, uiWidth;
    Deck deck;
    void blinkCard(Card* card);
    void paintEvent(QPaintEvent *event);
    virtual void afterPaintEvent(QPainter &painter) = 0;
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    // Draw fields and methods
    QPen bgPen, cardPen, titlePen;
    QFont cardFont, titleFont;
    void drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
                  bool shadow, int textX, int textY, int textHeight, int textWidth);
    void drawMana(QPainter &painter, QChar manaSymbol, int manaSize,
                  bool grayscale, int manaX, int manaY);

public:
    explicit DeckTrackerBase(QWidget *parent = nullptr);
    ~DeckTrackerBase();

signals:

public slots:
};

#endif // DECKTRACKERUI_H
