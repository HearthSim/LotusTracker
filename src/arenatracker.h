#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "entity/match.h"
#include "mtg/mtgarena.h"
#include "mtg/mtgcards.h"
#include "ui/decktrackerplayer.h"
#include "ui/decktrackeropponent.h"
#include "ui/preferencesscreen.h"
#include "ui/trayicon.h"
#include "utils/appsettings.h"
#include "utils/logger.h"

#include <QApplication>

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    bool isMatchRunning;
    DeckTrackerPlayer *deckTrackerPlayer;
    DeckTrackerOpponent *deckTrackerOpponent;
    TrayIcon *trayIcon;
    PreferencesScreen *preferencesScreen;
    void setupApp();
    void createPreferencesScreen();

public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    AppSettings *appSettings;
    Logger *logger;
    MtgArena *mtgArena;
    MtgCards *mtgCards;
    int run();
    void showPreferencesScreen();

signals:

private slots:
    void onNewMatchStart(Match match);
    void onDeckTrackerPlayerEnabledChange(bool enabled);
    void onDeckTrackerOpponentEnabledChange(bool enabled);
};

#endif // ARENATRACKER_H
