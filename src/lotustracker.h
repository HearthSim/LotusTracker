#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "api/lotusapi.h"
#include "entity/matchinfo.h"
#include "entity/opponentinfo.h"
#include "mtg/mtgarena.h"
#include "mtg/mtgcards.h"
#include "mtg/mtgdecksarch.h"
#include "mtg/mtgamatch.h"
#include "ui/decktrackerplayer.h"
#include "ui/decktrackeropponent.h"
#include "ui/preferencesscreen.h"
#include "ui/startscreen.h"
#include "ui/trayicon.h"
#include "utils/appsettings.h"
#include "utils/logger.h"
#include "updater/sparkleupdater.h"

#include <QApplication>
#include <QLocalServer>
#include <QTimer>

class LotusTracker : public QApplication
{
    Q_OBJECT

private:
    QLocalServer *localServer;
    DeckTrackerPlayer *deckTrackerPlayer;
    DeckTrackerOpponent *deckTrackerOpponent;
    TrayIcon *trayIcon;
    PreferencesScreen *preferencesScreen;
    StartScreen *startScreen;
    LotusTrackerAPI *lotusAPI;
    QPair<QString, Deck> eventPlayerCourse;
    QTimer *hideTrackerTimer, *checkConnection;
    bool isAlreadyRunning();
    bool isOnline();
    void setupApp();
    void setupUpdater();
    void setupPreferencesScreen();
    void setupLogParserConnections();
    void setupMtgaMatchConnections();
    void checkForAutoLogin();

public:
    LotusTracker(int& argc, char **argv);
    ~LotusTracker();
    AppSettings *appSettings;
    Logger *logger;
    MtgArena *mtgArena;
    MtgCards *mtgCards;
    MtgDecksArch *mtgDecksArch;
    MtgaMatch *mtgaMatch;
    SparkleUpdater *sparkleUpdater;
    int run();
    void avoidAppClose();
    void showStartScreen();
    void showPreferencesScreen();
    void showMessage(QString msg, QString title = tr("Lotus Tracker"));
    void publishOrUpdatePlayerDeck(Deck deck);

signals:

private slots:
    void onDeckSubmited(QString eventId, Deck deck);
    void onEventPlayerCourse(QString eventId, Deck currentDeck);
    void onMatchStart(QString eventId, OpponentInfo match);
    void onGameStart(MatchMode mode, QList<MatchZone> zones, int seatId);
    void onGameFocusChanged(bool hasFocus);
    void onGameCompleted(QMap<int, int> teamIdWins);
    void onMatchEnds(int winningTeamId);
    void onDeckTrackerPlayerEnabledChange(bool enabled);
    void onDeckTrackerOpponentEnabledChange(bool enabled);
    void onUserSigned(bool fromSignUp);
    void onUserTokenRefreshed();
    void onUserTokenRefreshError();
};

#endif // ARENATRACKER_H
