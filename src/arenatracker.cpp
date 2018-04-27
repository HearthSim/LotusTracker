#include "arenatracker.h"

ArenaTracker::ArenaTracker(int& argc, char **argv)
    : QApplication(argc, argv)
{
    setupApp();
    logger = new Logger(this);
    preferences = new Preferences();
    trayIcon = new TrayIcon(this);
    LOGI("Arena Tracker started");
    mtgArena = new MtgArena(this);
    mtgCards = new MtgCards(this);
}

ArenaTracker::~ArenaTracker()
{
    DELETE(logger)
    DELETE(preferences)
    DELETE(trayIcon)
    DELETE(mtgArena)
    DELETE(mtgCards)
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

void ArenaTracker::showPreferences()
{
    preferences->show();
    preferences->raise();
}
