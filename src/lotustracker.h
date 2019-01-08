#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include "api/lotusapi.h"
#include "entity/matchinfo.h"
#include "entity/opponentinfo.h"
#include "mtg/mtgarena.h"
#include "mtg/mtgcards.h"
#include "mtg/mtgdecksarch.h"
#include "mtg/mtgamatch.h"
#include "ui/deckoverlayplayer.h"
#include "ui/deckoverlayopponent.h"
#include "ui/deckoverlaydraft.h"
#include "ui/preferencesscreen.h"
#include "ui/startscreen.h"
#include "ui/trayicon.h"
#include "utils/appsettings.h"
#include "utils/logger.h"
#include "updater/sparkleupdater.h"
#include "credentials.h"
#include "ganalytics.h"

#include <QApplication>
#include <QLocalServer>
#include <QTimer>

class LotusTracker : public QApplication
{
    Q_OBJECT

private:
    QLocalServer *localServer;
    DeckOverlayPlayer *deckOverlayPlayer;
    DeckOverlayOpponent *deckOverlayOpponent;
    DeckOverlayDraft *deckOverlayDraft;
    TrayIcon *trayIcon;
    PreferencesScreen *preferencesScreen;
    StartScreen *startScreen;
    LotusTrackerAPI *lotusAPI;
    QPair<QString, Deck> eventPlayerCourse;
    QTimer *hideTrackerTimer, *checkConnection;
    bool isOnDraftScreen;
    bool isAlreadyRunning();
    bool isOnline();
    void setupApp();
    void setupUpdater();
    void setupPreferencesScreen();
    void setupLotusAPIConnectsions();
    void setupLogParserConnections();
    void setupMtgaMatchConnections();
    void checkForAutoLogin();

public:
    LotusTracker(int& argc, char **argv);
    ~LotusTracker();
    AppSettings* appSettings;
    Logger* logger;
    MtgArena* mtgArena;
    MtgCards* mtgCards;
    MtgDecksArch* mtgDecksArch;
    MtgaMatch* mtgaMatch;
    SparkleUpdater* sparkleUpdater;
    GAnalytics* gaTracker;
    int run();
    void avoidAppClose();
    void showStartScreen();
    void showPreferencesScreen();
    void showMessage(QString msg, QString title = tr("Lotus Tracker"));
    void publishOrUpdatePlayerDeck(Deck deck);

signals:

private slots:
    void onPlayerCollectionUpdated(QMap<int, int> ownedCards);
    void onPlayerDecks(QList<Deck> playerDecks);
    void onDeckSubmited(QString eventId, Deck deck);
    void onEventPlayerCourse(QString eventId, Deck currentDeck);
    void onMatchStart(QString eventId, OpponentInfo match);
    void onGameStart(MatchMode mode, QList<MatchZone> zones, int seatId);
    void onGameStarted();
    void onGameFocusChanged(bool hasFocus);
    void onGameStopped();
    void onGameCompleted(QMap<int, int> teamIdWins);
    void onMatchEnds(int winningTeamId);
    void onEventFinish(QString eventId, QString deckId, QString deckColors,
                       int maxWins, int wins, int losses);
    void onDeckOverlayDraftEnabledChange(bool enabled);
    void onDeckOverlayPlayerEnabledChange(bool enabled);
    void onDeckOverlayOpponentEnabledChange(bool enabled);
    void onUserSigned(bool fromSignUp);
    void onUserTokenRefreshed();
    void onUserTokenRefreshError();
    void onDraftStatus(QString eventId, QString status, int packNumber, int pickNumber,
                       QList<Card*> availablePicks, QList<Card*> pickedCards);
};

#endif // ARENATRACKER_H
