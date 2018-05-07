#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "logger.h"
#include "entity/match.h"
#include "mtg/mtgarena.h"
#include "mtg/mtgcards.h"
#include "ui/decktrackerplayer.h"
#include "ui/decktrackeropponent.h"
#include "ui/preferences.h"
#include "ui/trayicon.h"

#include <QApplication>

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    void setupApp();
    DeckTrackerPlayer *deckTrackerPlayer;
    DeckTrackerOpponent *deckTrackerOpponent;
    TrayIcon *trayIcon;
    Preferences *preferences;
    void onNewMatchStart(Match match);

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
