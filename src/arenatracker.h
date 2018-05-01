#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "logger.h"
#include "mtg/mtgarena.h"
#include "ui/decktrackeroverlay.h"
#include "ui/preferences.h"
#include "ui/trayicon.h"

#include <QApplication>

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    void setupApp();
    DeckTrackerOverlay *deckTrackerOverlay;
    TrayIcon *trayIcon;
    Preferences *preferences;

public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    Logger *logger;
    MtgArena *mtgArena;
    int run();
    void showPreferences();

signals:

public slots:
};

#endif // ARENATRACKER_H
