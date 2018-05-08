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
    trayIcon = new TrayIcon(this);
    createPreferencesScreen();
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchCreated,
            this, &ArenaTracker::onNewMatchStart);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckSelected,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDeckSelected);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDrawCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDrawCard);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnOpponentPlayCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPlayCard);
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

void ArenaTracker::createPreferencesScreen()
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

void ArenaTracker::showPreferencesScreen()
{
    preferencesScreen->show();
    preferencesScreen->raise();
}

void ArenaTracker::onNewMatchStart(Match match)
{
    UNUSED(match);
    isMatchRunning = true;
    if (APP_SETTINGS->isDeckTrackerPlayerEnabled()) {
        deckTrackerPlayer->show();
    }
    if (APP_SETTINGS->isDeckTrackerOpponentEnabled()) {
        deckTrackerOpponent->show();
    }
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
