#ifndef DECKTRACKEROPPONENT_H
#define DECKTRACKEROPPONENT_H

#include "decktrackerbase.h"
#include "../entity/card.h"

#include <QWidget>

class DeckTrackerOpponent : public DeckTrackerBase
{
    Q_OBJECT
private:
    int lastUiScale;
protected:
    QString onGetDeckColorIdentity();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter);
    void insertCard(Card* card);

public:
    explicit DeckTrackerOpponent(QWidget *parent = nullptr);
    ~DeckTrackerOpponent();
    void applyCurrentSettings();
    void clearDeck();

signals:

public slots:
    void onOpponentPutInLibraryCard(Card* card);
    void onOpponentPlayCard(Card* card);
    void onOpponentDiscardCard(Card* card);
    void onOpponentDiscardFromLibraryCard(Card* card);
    void onOpponentPutOnBattlefieldCard(Card* card);
};

#endif // DECKTRACKEROPPONENT_H
