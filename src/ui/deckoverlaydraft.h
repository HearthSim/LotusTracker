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
    QString currentSource, eventName;
    QFont rankFont;
    QList<Card *> availablePicks, pickedCards;
    QMap<int, int> playerCollection;
    QRect preferencesButton;
    QString getHoverCardRank();
    void udpateAvailableCardsList(QList<Card*> availablePicks, QList<Card*> pickedCards);

protected:    
    virtual QList<Card*> getDeckCardsSorted();
    virtual int getDeckNameYPosition();
    virtual int getHoverCardXPosition();
    virtual int cardHoverMarginBottom(QPainter &painter);
    virtual QString getDeckColorIdentity();
    virtual QString cardQtdFormat();
    virtual bool useGrayscaleForZeroQtd();
    virtual void onPositionChanged();
    virtual void onScaleChanged();
    virtual void beforeDrawCardEvent(QPainter &painter, Card* card, int cardBGY);
    virtual void drawHoverCard(QPainter &painter);
    virtual void afterPaintEvent(QPainter &painter);
    virtual void onHoverMove(QHoverEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DeckOverlayDraft(QWidget *parent = nullptr);
    ~DeckOverlayDraft();
    void applyCurrentSettings();
    void reset();
    QString getCurrentDraftName();
    QList<Card*> getAvailablePicks();

signals:
    void sgnRequestPlayerCollection();

public slots:
    void setPlayerCollection(QMap<int, int> ownedCards);
    void onDraftStatus(QString eventName, QList<Card*> availablePicks, QList<Card*> pickedCards);
    void onSourceChanged(QString source);
};

#endif // DECKOVERLAYDRAFT_H
