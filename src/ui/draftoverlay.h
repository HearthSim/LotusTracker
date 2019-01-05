#ifndef DRAFTOVERLAY_H
#define DRAFTOVERLAY_H

#include "deckoverlaybase.h"
#include "../entity/card.h"

#include <QWidget>

class DraftOverlay : public DeckOverlayBase
{
    Q_OBJECT
private:
    QMap<int, int> playerCollection;
    QRect preferencesButton;

protected:    
    virtual QList<Card*> getDeckCardsSorted();
    virtual int getDeckNameYPosition();
    virtual int getHoverCardXPosition();
    virtual QString getDeckColorIdentity();
    virtual QString cardQtdFormat();
    virtual bool useGrayscaleForZeroQtd();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void afterPaintEvent(QPainter &painter);
    virtual void onHoverMove(QHoverEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DraftOverlay(QWidget *parent = nullptr);
    ~DraftOverlay();
    void applyCurrentSettings();
    void reset();

signals:

public slots:
    void setPlayerCollection(QMap<int, int> ownedCards);
    void onDraftStatus(QList<Card*> availablePicks, QList<Card*> pickedCards);
};

#endif // DRAFTOVERLAY_H
