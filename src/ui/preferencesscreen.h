#ifndef PREFERENCESSCREEN_H
#define PREFERENCESSCREEN_H

#include <QCloseEvent>
#include <QMainWindow>

namespace Ui { class Preferences; }

class PreferencesScreen : public QMainWindow
{
    Q_OBJECT

private:
    void closeEvent(QCloseEvent *event);
    void onStartAtLoginChanged();
    void onCardLayoutChanged();
    void onPTEnabledChanged();
    void onPTStatisticsChanged();
    void onOTEnabledChanged();
    Ui::Preferences *ui;

public:
    explicit PreferencesScreen(QWidget *parent = 0);
    ~PreferencesScreen();

signals:
    void sgnTrackerCardLayout(QString cardLayout);
    void sgnPlayerTrackerEnabled(bool enabled);
    void sgnPlayerTrackerStatistics(bool enabled);
    void sgnOpponentTrackerEnabled(bool enabled);

};

#endif // PREFERENCESSCREEN_H
