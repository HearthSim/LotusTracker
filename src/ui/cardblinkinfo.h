#ifndef BLINKINFO_H
#define BLINKINFO_H

#include "../entity/card.h"

#include <QObject>
#include <QMainWindow>
#include <QTimer>

class CardBlinkInfo : public QObject
{
    Q_OBJECT

public:
    QMainWindow *parentQMainWindow;
    Card* card;
    QTimer* timer;
    int alpha;
    int acc;

    CardBlinkInfo(QMainWindow *parentQMainWindow, Card* card, QTimer* timer):
        parentQMainWindow(parentQMainWindow), card(card), timer(timer), alpha(200){
        acc = 0;
    }

    ~CardBlinkInfo()
    {
        if (timer) {
            if (timer->isActive()) {
                timer->stop();
            }
            delete timer;
        }
    }

public slots:
    void timeout(){
        acc += 3;
        alpha -= 5 + acc;
        if (alpha <= 0) {
            acc = 0;
            alpha = 0;
            timer->stop();
        }
        parentQMainWindow->update();
    }
};

#endif // BLINKINFO_H
