#ifndef DECKTRACKERPLAYER_H
#define DECKTRACKERPLAYER_H

#include "decktrackerbase.h"
#include "../entity/card.h"
#include "../entity/deck.h"

#include <QTimer>
#include <QWidget>

class DeckTrackerPlayer : public DeckTrackerBase
{
    Q_OBJECT
private:
    QTimer *publishDeckTimer;
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
    virtual QString onGetDeckColorIdentity();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter);
    virtual void onHoverMove(QHoverEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DeckTrackerPlayer(QWidget *parent = nullptr);
    ~DeckTrackerPlayer();
    void stopPublishDeckAnimation();
    void applyCurrentSettings();
    void loadDeck(Deck deck);
    void loadDeckWithSideboard(QMap<Card*, int> cards);
    void resetDeck();
    bool isDeckLoadedAndReseted();

signals:

public slots:
    void onLotusAPIRequestFinishedWithSuccess();
    void onLotusAPIRequestFinishedWithError();
    void onPlayerPutInLibraryCard(Card* card);
    void onPlayerDrawCard(Card* card);
    void onPlayerDeckStatus(int wins, int losses, double winRate);
    void onReceiveEventName(QString name);
    void onPlayerDiscardCard(Card* card);
    void onPlayerDiscardFromLibraryCard(Card* card);
    void onPlayerPutOnBattlefieldCard(Card* card);
    void onShowOnlyRemainingCardsEnabled(bool enabled);
    void onStatisticsEnabled(bool enabled);
};

#endif // DECKTRACKERPLAYER_H
