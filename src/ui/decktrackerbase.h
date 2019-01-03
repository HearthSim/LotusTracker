#ifndef DECKTRACKERUI_H
#define DECKTRACKERUI_H

#include "../entity/card.h"
#include "../entity/deck.h"
#include "cardblinkinfo.h"

#include <QMainWindow>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QNetworkAccessManager>

namespace Ui { class TrackerOverlay; }

class DeckTrackerBase : public QMainWindow
{
    Q_OBJECT
private:
    Ui::TrackerOverlay *ui;
    QString cardBGSkin, cachesDir;
    QRect cardsRect, expandBar;
    int currentHoverPosition, unhiddenTimeout, stackCardsPixels;
    Card* hoverCard;
    QTimer* unhiddenTimer;
    QNetworkAccessManager networkManager;
    bool mousePressed;
    QPoint mouseInitialPosition;
    void setupWindow();
    // Draw fields and methods
    QMap<Card*, CardBlinkInfo*> cardsBlinkInfo;
    void setupDrawTools();
    int getCardHeight();
    QList<Card*> getDeckCardsSorted();
    void onHoverEnter(QHoverEvent *event);
    void onHoverLeave(QHoverEvent *event);
    int getCardsHoverPosition(QHoverEvent *event);
    void updateCardHoverUrl(int hoverPosition);
    void onRightClick();
    void onExpandBarClick();
    void onZoomMinusClick();
    void onZoomPlusClick();
    void onCardImageDownloaded();

protected:
    const int cornerRadius;
    QPoint uiPos;
    QRect zoomMinusButton, zoomPlusButton, screen;
    qreal uiAlpha;
    int cardHoverWidth, titleHeight, uiHeight, uiWidth, uiScale;
    Deck deck;
    bool hidden, isShowCardManaCostEnabled, isShowCardOnHoverEnabled, showingTooltip;
    void blinkCard(Card* card);
    void hideCardOnHover();
    void paintEvent(QPaintEvent *event);
    void drawCover(QPainter &painter);
    void drawCoverButtons(QPainter &painter);
    void drawDeckInfo(QPainter &painter);
    void drawDeckCards(QPainter &painter);
    void drawExpandBar(QPainter &painter);
    void drawHoverCard(QPainter &painter);
    virtual int getDeckNameYPosition() = 0;
    virtual int getHoverCardXPosition() = 0;
    virtual QString getDeckColorIdentity() = 0;
    virtual void onPositionChanged() = 0;
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter) = 0;
    virtual bool event(QEvent *event);
    virtual void onHoverMove(QHoverEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    // Draw fields and methods
    QPen bgPen, cardPen, cardNonePen, titlePen;
    QFont cardFont, titleFont;
    void drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
                  bool shadow, int textX, int textY, int textHeight, int textWidth);
    void drawMana(QPainter &painter, QString manaSymbol, int manaSize,
                  bool grayscale, int manaX, int manaY);

public:
    explicit DeckTrackerBase(QWidget *parent = nullptr);
    ~DeckTrackerBase();
    Deck getDeck();

    static const QString TITLE(){ return "TrackerOverlay"; }

signals:

public slots:
    void changeAlpha(int alpha);
    void changeCardLayout(QString cardLayout);
    void onShowCardManaCostEnabled(bool enabled);
    void onShowCardOnHoverEnabled(bool enabled);
    void changeUnhiddenTimeout(int unhiddenTimeout);
};

#endif // DECKTRACKERUI_H
