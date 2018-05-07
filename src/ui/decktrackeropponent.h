#ifndef DECKTRACKEROPPONENT_H
#define DECKTRACKEROPPONENT_H

#include "decktrackerbase.h"
#include "../entity/card.h"

#include <QWidget>

class DeckTrackerOpponent : public DeckTrackerBase
{
    Q_OBJECT

public:
    explicit DeckTrackerOpponent(QWidget *parent = nullptr);
    ~DeckTrackerOpponent();

signals:

public slots:
    void onOpponentPlayCard(Card* card);
};

#endif // DECKTRACKEROPPONENT_H
