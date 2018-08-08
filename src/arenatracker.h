#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "entity/matchinfo.h"
#include "entity/opponentinfo.h"
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
#include "firebase/database.h"
#include "updater/sparkleupdater.h"

#include <QApplication>
#include <QLocalServer>

class ArenaTracker : public QApplication
{
    Q_OBJECT

private:
    QLocalServer *localServer;
    DeckTrackerPlayer *deckTrackerPlayer;
    DeckTrackerOpponent *deckTrackerOpponent;
    TrayIcon *trayIcon;
    PreferencesScreen *preferencesScreen;
    StartScreen *startScreen;
    FirebaseAuth *firebaseAuth;
    FirebaseDatabase *firebaseDatabase;
    QPair<QString, Deck> eventPlayerCourse;
    bool isAlreadyRunning();
    void setupApp();
    void setupUpdater();
    void setupPreferencesScreen();
    void setupLogParserConnections();
    void setupMtgaMatch();
    void checkForAutoLogin();

public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    AppSettings *appSettings;
    Logger *logger;
    MtgArena *mtgArena;
    MtgCards *mtgCards;
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
    void onEventPlayerCourse(QString eventId, Deck currentDeck);
    void onMatchStart(QString eventId, OpponentInfo match);
    void onGameStart(MatchMode mode, QList<MatchZone> zones);
    void onGameFocusChanged(bool hasFocus);
    void onGameCompleted(QMap<int, int> teamIdWins);
    void onMatchEnds(int winningTeamId);
    void onPlayerTakesMulligan();
    void onDeckTrackerPlayerEnabledChange(bool enabled);
    void onDeckTrackerOpponentEnabledChange(bool enabled);
    void onUserSigned(bool fromSignUp);
    void onUserTokenRefreshed();
    void onUserTokenRefreshError();
};

#endif // ARENATRACKER_H
