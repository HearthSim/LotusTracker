#ifndef DECKTRACKEROVERLAY_H
#define DECKTRACKEROVERLAY_H

#include "decktrackerui.h"

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>

namespace Ui { class DeckTracker; }

class DeckTrackerOverlay : public QMainWindow
{
    Q_OBJECT

private:
    Ui::DeckTracker *ui;
    QRect screen;
    DeckTrackerUI *playerDeckTrackerUI;
    DeckTrackerUI *opponentDeckTrackerUI;

protected:
    void setupWindow();
    void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public:
    explicit DeckTrackerOverlay(QWidget *parent = nullptr);
    ~DeckTrackerOverlay();

signals:

public slots:
};

#endif // DECKTRACKEROVERLAY_H
