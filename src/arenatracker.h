#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "macros.h"
#include "logger.h"
#include "trayicon.h"
#include "ui/preferences.h"

class ArenaTracker : public QApplication
{
    Q_OBJECT
public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    Logger *logger;
    int run();
    void showPreferences();

private:
    void setupApp();
    TrayIcon *trayIcon;
    Preferences *preferences;

signals:

public slots:
};

#endif // ARENATRACKER_H
