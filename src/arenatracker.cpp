#include "arenatracker.h"
#include "macros.h"
#include "mtg/mtgalogparser.h"

ArenaTracker::ArenaTracker(int& argc, char **argv): QApplication(argc, argv),
    isMatchRunning(false)
{
    setupApp();
    logger = new Logger(this);
    appSettings = new AppSettings(this);
    mtgCards = new MtgCards(this);
    mtgArena = new MtgArena(this, mtgCards);
    deckTrackerPlayer = new DeckTrackerPlayer();
    deckTrackerOpponent = new DeckTrackerOpponent();
    trayIcon = new TrayIcon(this, mtgCards);
    auth = new Auth(this);
    connect(auth, &Auth::sgnUserLogged, this, &ArenaTracker::onUserCreated);
    setupMtgaMatch();
    setupPreferencesScreen();
    setupStartScreen();
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchCreated,
            this, &ArenaTracker::onMatchStart);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoResultMatch,
            this, &ArenaTracker::onMatchEnd);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckSubmited,
            this, &ArenaTracker::onDeckSubmited);
    LOGI("Arena Tracker started");
}

ArenaTracker::~ArenaTracker()
{
    DEL(logger)
    DEL(deckTrackerPlayer)
    DEL(deckTrackerOpponent)
    DEL(preferencesScreen)
    DEL(trayIcon)
    DEL(mtgArena)
    DEL(mtgaMatch)
    DEL(auth)
}

int ArenaTracker::run()
{
    return exec();
}

void ArenaTracker::setupApp()
{
#if defined Q_OS_MAC
  setAttribute(Qt::AA_UseHighDpiPixmaps);
  QIcon icon(":/res/icon_black.png");
  icon.addFile(":/res/icon_black@2x.png");
#elif defined Q_OS_WIN
  QIcon icon(":/res/icon.ico");
#endif
  setAttribute(Qt::AA_Use96Dpi);
  setApplicationName("Arena Tracker");
  setOrganizationName("ArenaMeta");
  setOrganizationDomain("arenameta.com");
  setWindowIcon(icon);
}

void ArenaTracker::setupPreferencesScreen()
{
    preferencesScreen = new PreferencesScreen();
    // Deck tracker player
    connect(preferencesScreen, &PreferencesScreen::sgnPlayerTrackerEnabled,
            this, &ArenaTracker::onDeckTrackerPlayerEnabledChange);
    connect(preferencesScreen, &PreferencesScreen::sgnTrackerCardLayout,
            deckTrackerPlayer, &DeckTrackerPlayer::changeCardLayout);
    connect(preferencesScreen, &PreferencesScreen::sgnPlayerTrackerStatistics,
            deckTrackerPlayer, &DeckTrackerPlayer::onStatisticsEnabled);
    // Deck tracker opponent
    connect(preferencesScreen, &PreferencesScreen::sgnOpponentTrackerEnabled,
            this, &ArenaTracker::onDeckTrackerOpponentEnabledChange);
    connect(preferencesScreen, &PreferencesScreen::sgnTrackerCardLayout,
            deckTrackerOpponent, &DeckTrackerPlayer::changeCardLayout);
}

void ArenaTracker::setupStartScreen()
{
    startScreen = new StartScreen(nullptr, auth);
    UserSettings userSettings = appSettings->getUserSettings();
    if (userSettings.isValid()) {
        emit auth->sgnUserLogged(userSettings);
    } else {
        startScreen->show();
        startScreen->raise();
    }
}

void ArenaTracker::setupMtgaMatch()
{
    mtgaMatch = new MtgaMatch(this, mtgCards);
    // Player
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutInLibraryCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerPutInLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDrawCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDrawCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDiscardCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDiscardCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDiscardFromLibraryCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDiscardFromLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutOnBattlefieldCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerPutOnBattlefieldCard);
    // Opponent
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutInLibraryCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPutInLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPlayCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPlayCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentDiscardCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentDiscardCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentDiscardFromLibraryCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentDiscardFromLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutOnBattlefieldCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPutOnBattlefieldCard);
    // Match
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoSeats,
            mtgaMatch, &MtgaMatch::onMatchInfoSeats);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnSeatIdThatGoFirst,
            mtgaMatch, &MtgaMatch::onSeatIdThatGoFirst);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchStartZones,
            mtgaMatch, &MtgaMatch::onMatchStartZones);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchStateDiff,
            mtgaMatch, &MtgaMatch::onMatchStateDiff);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnNewTurnStarted,
            mtgaMatch, &MtgaMatch::onNewTurnStarted);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerTakesMulligan,
            mtgaMatch, &MtgaMatch::onPlayerTakesMulligan);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnOpponentTakesMulligan,
            mtgaMatch, &MtgaMatch::onOpponentTakesMulligan);
}

void ArenaTracker::showStartScreen()
{
    startScreen->show();
    startScreen->raise();
}

void ArenaTracker::showPreferencesScreen()
{
    preferencesScreen->show();
    preferencesScreen->raise();
}

void ArenaTracker::showMessage(QString msg, QString title)
{
    trayIcon->showMessage(title, msg);
}

void ArenaTracker::onDeckSubmited(Deck deck)
{
    deckTrackerPlayer->loadDeck(deck);
    if (APP_SETTINGS->isDeckTrackerPlayerEnabled()) {
        deckTrackerPlayer->show();
    }
    if (APP_SETTINGS->isDeckTrackerOpponentEnabled()) {
        deckTrackerOpponent->show();
    }
}

void ArenaTracker::onMatchStart(MatchInfo matchInfo)
{
    isMatchRunning = true;
    mtgaMatch->startNewMatch(matchInfo);
}

void ArenaTracker::onMatchEnd(int winningTeamId)
{
    isMatchRunning = false;
    deckTrackerPlayer->hide();
    deckTrackerOpponent->hide();
    mtgaMatch->endCurrentMatch(winningTeamId);
}

void ArenaTracker::onDeckTrackerPlayerEnabledChange(bool enabled)
{
    if (enabled && isMatchRunning) {
        deckTrackerPlayer->show();
    }
    if (!enabled && isMatchRunning) {
        deckTrackerPlayer->hide();
    }
}

void ArenaTracker::onDeckTrackerOpponentEnabledChange(bool enabled)
{
    if (enabled && isMatchRunning) {
        deckTrackerOpponent->show();
    }
    if (!enabled && isMatchRunning) {
        deckTrackerOpponent->hide();
    }
}

void ArenaTracker::onUserCreated(UserSettings userSettings)
{
    startScreen->hide();
    trayIcon->updateUserSettings(userSettings);
}
