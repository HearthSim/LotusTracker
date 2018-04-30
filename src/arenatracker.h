#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "macros.h"
#include "mtg/mtgarena.h"
#include "mtg/mtgcards.h"
#include "ui/decktrackeroverlay.h"
#include "ui/preferences.h"
#include "ui/trayicon.h"

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
    MtgCards *mtgCards;
    int run();
    void showPreferences();

signals:

public slots:
};

#endif // ARENATRACKER_H
