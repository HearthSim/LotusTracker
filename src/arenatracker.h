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
#include "updater/sparkleupdater.h"

#include <QApplication>
#include <QLocalServer>

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    QLocalServer *localServer;
    bool isMatchRunning;
    MtgCards *mtgCards;
    DeckTrackerPlayer *deckTrackerPlayer;
    DeckTrackerOpponent *deckTrackerOpponent;
    TrayIcon *trayIcon;
    PreferencesScreen *preferencesScreen;
    StartScreen *startScreen;
    Auth *auth;
    bool isAlreadyRunning();
    void setupApp();
    void setupUpdater();
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
    SparkleUpdater *sparkleUpdater;
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
    void onUserTokenRefreshError();
};

#endif // ARENATRACKER_H
