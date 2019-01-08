#ifndef DECKOVERLAYOPPONENT_H
#define DECKOVERLAYOPPONENT_H

#include "deckoverlaybase.h"
#include "../entity/card.h"

#include <QWidget>

class DeckOverlayOpponent : public DeckOverlayBase
{
    Q_OBJECT
private:
    int lastUiScale;
    QString eventId, eventType;

protected:
    virtual int getDeckNameYPosition();
    virtual int getHoverCardXPosition();
    virtual QString getDeckColorIdentity();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter);
    void insertCard(Card* card);

public:
    explicit DeckOverlayOpponent(QWidget *parent = nullptr);
    ~DeckOverlayOpponent();
    void applyCurrentSettings();
    void setEventId(QString eventId);
    void reset();

signals:

public slots:    
    void onReceiveEventInfo(QString name, QString type);
    void onOpponentPutInLibraryCard(Card* card);
    void onOpponentPlayCard(Card* card);
    void onOpponentDiscardCard(Card* card);
    void onOpponentDiscardFromLibraryCard(Card* card);
    void onOpponentPutOnBattlefieldCard(Card* card);
};

#endif // DECKOVERLAYOPPONENT_H
