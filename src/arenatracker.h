#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "entity/matchinfo.h"
#include "mtg/mtgarena.h"
#include "mtg/mtgcards.h"
#include "mtg/mtgamatch.h"
#include "ui/decktrackerplayer.h"
#include "ui/decktrackeropponent.h"
#include "ui/preferencesscreen.h"
#include "ui/startscreen.h"
#include "ui/trayicon.h"
#include "utils/appsettings.h"
#include "utils/logger.h"
#include "firebase/auth.h"

#include <QApplication>

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    bool isMatchRunning;
    MtgCards *mtgCards;
    DeckTrackerPlayer *deckTrackerPlayer;
    DeckTrackerOpponent *deckTrackerOpponent;
    TrayIcon *trayIcon;
    PreferencesScreen *preferencesScreen;
    StartScreen *startScreen;
    Auth *auth;
    void setupApp();
    void setupPreferencesScreen();
    void setupMtgaMatch();
    void checkForAutoLogin();

public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    AppSettings *appSettings;
    Logger *logger;
    MtgArena *mtgArena;
    MtgaMatch *mtgaMatch;
    int run();
    void avoidAppClose();
    void showStartScreen();
    void showPreferencesScreen();
    void showMessage(QString msg, QString title = tr("Arena Tracker"));

signals:

private slots:
    void onDeckSubmited(Deck deck);
    void onMatchStart(MatchInfo match);
    void onMatchEnd(int winningTeamId);
    void onDeckTrackerPlayerEnabledChange(bool enabled);
    void onDeckTrackerOpponentEnabledChange(bool enabled);
    void onUserSigned(bool fromSignUp);
    void onUserTokenRefreshed();
};

#endif // ARENATRACKER_H
