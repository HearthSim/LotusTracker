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
    QRect cardsRect, expandBar, zoomMinusButton, zoomPlusButton;
    int currentHoverPosition, hoverCardMultiverseId, unhiddenTimeout;
    QTimer* unhiddenTimer;
    QNetworkAccessManager networkManager;
    bool mousePressed;
    QPoint mouseRelativePosition;
    void setupWindow();
    // Draw fields and methods
    QMap<Card*, CardBlinkInfo*> cardsBlinkInfo;
    void setupDrawTools();
    int getCardHeight();
    QList<Card*> getDeckCardsSorted();
    void onHoverEnter(QHoverEvent *event);
    void onHoverMove(QHoverEvent *event);
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
    qreal uiAlpha, uiScale;
    int cardHoverWidth, uiHeight, uiWidth;
    Deck deck;
    bool hidden, showCardOnHover;
    void blinkCard(Card* card);
    void paintEvent(QPaintEvent *event);
    void drawCover(QPainter &painter);
    void drawCoverButtons(QPainter &painter);
    void drawDeckInfo(QPainter &painter);
    void drawDeckCards(QPainter &painter);
    void drawExpandBar(QPainter &painter);
    void drawHoverCard(QPainter &painter);
    virtual int onGetZoomPlusButtonX() = 0;
    virtual QString onGetDeckColorIdentity() = 0;
    virtual void onPositionChanged() = 0;
    virtual void onScaleChanged() = 0;
    virtual void afterPaintEvent(QPainter &painter) = 0;
    virtual bool event(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    // Draw fields and methods
    QPen bgPen, cardPen, cardNonePen, titlePen;
    QFont cardFont, titleFont;
    void drawText(QPainter &painter, QFont textFont, QPen textPen, QString text, int textOptions,
                  bool shadow, int textX, int textY, int textHeight, int textWidth);
    void drawMana(QPainter &painter, QChar manaSymbol, int manaSize,
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
    void onShowCardOnHoverEnabled(bool enabled);
    void changeUnhiddenTimeout(int unhiddenTimeout);
};

#endif // DECKTRACKERUI_H
