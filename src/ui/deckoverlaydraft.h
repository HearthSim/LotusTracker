#ifndef DECKOVERLAYDRAFT_H
#define DECKOVERLAYDRAFT_H

#include "deckoverlaybase.h"
#include "../entity/card.h"

#include <QWidget>

class DeckOverlayDraft : public DeckOverlayBase
{
    Q_OBJECT
private:
    int rankDescTextMargin;
    QFont rankFont;
    QList<Card *> availablePicks;
    QMap<int, int> playerCollection;
    QRect preferencesButton;
    void udpateAvailableCardsList(QList<Card *> availablePicks);

protected:    
    virtual QList<Card*> getDeckCardsSorted();
    virtual int getDeckNameYPosition();
    virtual int getHoverCardXPosition();
    virtual QString getDeckColorIdentity();
    virtual QString cardQtdFormat();
    virtual bool useGrayscaleForZeroQtd();
    virtual void onPositionChanged();
    virtual void onScaleChanged();    
    virtual void beforeDrawCardEvent(QPainter &painter, Card* card, int cardBGY);
    virtual void afterPaintEvent(QPainter &painter);
    virtual void onHoverMove(QHoverEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DeckOverlayDraft(QWidget *parent = nullptr);
    ~DeckOverlayDraft();
    void applyCurrentSettings();
    void reset();

signals:
    void sgnRequestPlayerCollection();

public slots:
    void setPlayerCollection(QMap<int, int> ownedCards);
    void onDraftStatus(QList<Card*> availablePicks, QList<Card*> pickedCards);
};

#endif // DECKOVERLAYDRAFT_H
