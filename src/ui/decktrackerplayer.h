#ifndef DECKTRACKERPLAYER_H
#define DECKTRACKERPLAYER_H

#include "decktrackerbase.h"
#include "../entity/card.h"
#include "../entity/deck.h"

#include <QWidget>

class DeckTrackerPlayer : public DeckTrackerBase
{
    Q_OBJECT
private:
    bool isStatisticsEnabled;
    int deckWins, deckLosses;
    double deckWinRate;
    QPen bgPen, statisticsPen, winRatePen;
    QFont statisticsFont, winRateFont;
    QRect preferencesButton;
    void drawStatistics(QPainter &painter);

protected:
    virtual int onGetZoomPlusButtonX();
    virtual QString onGetDeckColorIdentity();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DeckTrackerPlayer(QWidget *parent = nullptr);
    ~DeckTrackerPlayer();
    void applyCurrentSettings();
    void loadDeck(Deck deck);
    void loadDeckWithSideboard(QMap<Card*, int> cards);
    void resetDeck();
    bool isDeckLoadedAndReseted();

signals:

public slots:
    void onPlayerPutInLibraryCard(Card* card);
    void onPlayerDrawCard(Card* card);
    void onPlayerDeckStatus(int wins, int losses, double winRate);
    void onPlayerDiscardCard(Card* card);
    void onPlayerDiscardFromLibraryCard(Card* card);
    void onPlayerPutOnBattlefieldCard(Card* card);
    void onShowOnlyRemainingCardsEnabled(bool enabled);
    void onStatisticsEnabled(bool enabled);
};

#endif // DECKTRACKERPLAYER_H
