#ifndef PREFERENCESSCREEN_H
#define PREFERENCESSCREEN_H

#include "tabgeneral.h"
#include "taboverlay.h"
#include "tablogs.h"
#include "tababout.h"

#include <QMainWindow>

#define NUM_TABS 4

namespace Ui { class Preferences; }

class PreferencesScreen : public QMainWindow
{
    Q_OBJECT

private:
    void closeEvent(QCloseEvent *event);
    Ui::Preferences *ui;
    TabGeneral *tabGeneral;
    TabOverlay *tabOverlay;
    TabLogs *tabLogs;
    TabAbout *tabAbout;
    QWidget *tabs[NUM_TABS];

public:
    explicit PreferencesScreen(QWidget *parent = nullptr);
    ~PreferencesScreen();
    TabGeneral* getTabGeneral();
    TabOverlay* getTabOverlay();
    TabLogs* getTabLogs();
    TabAbout* getTabAbout();

    static const QString TITLE(){ return "Preferences"; }

private slots:
    void tabClick(QAction *action);

};

#endif // PREFERENCESSCREEN_H
