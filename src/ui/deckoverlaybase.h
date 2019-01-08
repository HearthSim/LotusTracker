#ifndef DECKOVERLAYUI_H
#define DECKOVERLAYUI_H

#include "../entity/card.h"
#include "../entity/deck.h"
#include "cardblinkinfo.h"

#include <QMainWindow>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QNetworkAccessManager>

namespace Ui { class TrackerOverlay; }

class DeckOverlayBase : public QMainWindow
{
    Q_OBJECT
private:
    Ui::TrackerOverlay *ui;
    QString cardBGSkin, cachesDir;
    QRect cardsRect, expandBar;
    int unhiddenTimeout, stackCardsPixels;
    QTimer* unhiddenTimer;
    QNetworkAccessManager networkManager;
    bool mousePressed;
    QPoint mouseInitialPosition;
    void setupWindow();
    // Draw fields and methods
    QMap<Card*, CardBlinkInfo*> cardsBlinkInfo;
    void setupDrawTools();
    void onHoverEnter(QHoverEvent *event);
    void onHoverLeave(QHoverEvent *event);
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
    int cardHoverHeight, cardHoverWidth, titleHeight, coverHeight,
        uiHeight, uiWidth, uiScale, currentHoverPosition;
    Card* hoverCard;
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
    virtual void drawHoverCard(QPainter &painter);
    virtual QList<Card*> getDeckCardsSorted();
    virtual int getDeckNameYPosition() = 0;
    virtual int getHoverCardXPosition() = 0;
    virtual int cardHoverMarginBottom(QPainter &painter);
    virtual QString getDeckColorIdentity() = 0;
    virtual QString cardQtdFormat();
    virtual bool useGrayscaleForZeroQtd();
    virtual void onPositionChanged() = 0;
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter) = 0;
    virtual void beforeDrawCardEvent(QPainter &painter, Card* card, int cardBGY);
    virtual bool event(QEvent *event);
    virtual void onHoverMove(QHoverEvent *event);
    int getCardHeight();
    int getCardsHoverPosition(QHoverEvent *event);
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
    explicit DeckOverlayBase(QWidget *parent = nullptr);
    ~DeckOverlayBase();
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

#endif // DECKOVERLAYUI_H
