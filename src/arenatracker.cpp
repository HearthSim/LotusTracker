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
    setupMtgaMatch();
    setupPreferencesScreen();
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchCreated,
            this, &ArenaTracker::onNewMatchStart);
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

void ArenaTracker::setupMtgaMatch()
{
    mtgaMatch = new MtgaMatch(this, mtgCards);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerUndrawCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerUndrawCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDrawCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDrawCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPlayCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPlayCard);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoSeats,
            mtgaMatch, &MtgaMatch::onMatchInfoSeats);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoResultMatch,
            mtgaMatch, &MtgaMatch::onMatchInfoResultMatch);
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

void ArenaTracker::showPreferencesScreen()
{
    preferencesScreen->show();
    preferencesScreen->raise();
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

void ArenaTracker::onNewMatchStart(MatchInfo matchInfo)
{
    isMatchRunning = true;
    mtgaMatch->startNewMatch(matchInfo);
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
