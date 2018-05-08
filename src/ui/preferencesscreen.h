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
    Ui::Preferences *ui;

public:
    explicit PreferencesScreen(QWidget *parent = 0);
    ~PreferencesScreen();

signals:
    void sgnTrackerCardLayout(QString cardLayout);

};

#endif // PREFERENCESSCREEN_H
