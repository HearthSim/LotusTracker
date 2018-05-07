#include "arenatracker.h"
#include "macros.h"
#include "mtg/mtgalogparser.h"

ArenaTracker::ArenaTracker(int& argc, char **argv)
    : QApplication(argc, argv)
{
    setupApp();
    logger = new Logger(this);
    appSettings = new AppSettings(this);
    mtgCards = new MtgCards(this);
    mtgArena = new MtgArena(this, mtgCards);
    deckTrackerPlayer = new DeckTrackerPlayer();
    deckTrackerOpponent = new DeckTrackerOpponent();
    preferencesScreen = new PreferencesScreen();
    trayIcon = new TrayIcon(this);
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

int ArenaTracker::run()
{
    return exec();
}


void ArenaTracker::onNewMatchStart(Match match)
{
    UNUSED(match)
    deckTrackerPlayer->show();
    deckTrackerOpponent->show();
}

void ArenaTracker::showPreferences()
{
    preferencesScreen->show();
    preferencesScreen->raise();
}
