#ifndef TABOVERLAY_H
#define TABOVERLAY_H

#include <QWidget>

namespace Ui { class TabOverlay; }

class TabOverlay : public QWidget
{
    Q_OBJECT

private:
    Ui::TabOverlay *ui;
    void applyCurrentSettings();
    void onCardLayoutChanged();
    void onShowCardOnHoverChanged();
    void onShowOnlyRemainingCardsChanged();
    void onTrackerAlphaChanged();
    void onUnhideDelayChanged();
    void onPTStatisticsChanged();

public:
    explicit TabOverlay(QWidget *parent = nullptr);
    ~TabOverlay();

signals:
    void sgnTrackerAlpha(qreal alpha);
    void sgnUnhideDelay(int delay);
    void sgnTrackerCardLayout(QString cardLayout);
    void sgnShowCardOnHoverEnabled(bool enabled);
    void sgnShowOnlyRemainingCardsEnabled(bool enabled);
    void sgnPlayerTrackerStatistics(bool enabled);

public slots:
    void onRestoreDefaultsSettings();

};

#endif // TABOVERLAY_H
