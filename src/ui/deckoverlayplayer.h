#ifndef DECKOVERLAYPLAYER_H
#define DECKOVERLAYPLAYER_H

#include "deckoverlaybase.h"
#include "../entity/card.h"
#include "../entity/deck.h"

#include <QMenu>
#include <QTimer>
#include <QWidget>

class DeckOverlayPlayer : public DeckOverlayBase
{
    Q_OBJECT
private:
    QMenu *deckMenu;
    QTimer *publishDeckTimer;
    QAction *deckProfileAction;
    QAction *deckPublicProfileAction;
    QString publishingDeckIcon, eventName;
    bool isStatisticsEnabled;
    int deckWins, deckLosses, winrateFontSize;
    double deckWinRate;
    QPen statisticsPen, winRatePen;
    QFont statisticsFont, winRateFont;
    QRect publishDeckButton, preferencesButton;
    void publishingDeckAnim();
    void drawStatistics(QPainter &painter);

protected:
    virtual int getDeckNameYPosition();
    virtual int getHoverCardXPosition();
    virtual QString getDeckColorIdentity();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter);
    virtual void onHoverMove(QHoverEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DeckOverlayPlayer(QWidget *parent = nullptr);
    ~DeckOverlayPlayer();
    void stopPublishDeckAnimation();
    void applyCurrentSettings();
    void loadDeck(Deck deck);
    void loadDeckWithSideboard(QMap<Card*, int> cards, QMap<Card*, int> sideboard);
    void reset();
    bool isDeckLoadedAndReseted();

signals:

public slots:
    void onLotusAPIRequestFinishedWithSuccess();
    void onLotusAPIRequestFinishedWithError();
    void onPlayerPutOnLibraryCard(Card* card);
    void onPlayerPutOnHandCard(Card* card);
    void onPlayerDrawCard(Card* card);
    void onPlayerDeckStatus(int wins, int losses, double winRate);
    void onReceiveEventInfo(QString name, QString type);
    void onPlayerDiscardCard(Card* card);
    void onPlayerDiscardFromLibraryCard(Card* card);
    void onPlayerPutOnBattlefieldCard(Card* card);
    void onShowOnlyRemainingCardsEnabled(bool enabled);
    void onStatisticsEnabled(bool enabled);
};

#endif // DECKOVERLAYPLAYER_H
