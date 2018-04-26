#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "macros.h"
#include "mtgarena.h"
#include "mtgcards.h"
#include "logger.h"
#include "trayicon.h"
#include "ui/preferences.h"

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    void setupApp();
    TrayIcon *trayIcon;
    Preferences *preferences;

public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    Logger *logger;
    MtgArena *mtgArena;
    MtgCards *mtgCards;
    int run();
    void showPreferences();

signals:

public slots:
};

#endif // ARENATRACKER_H
