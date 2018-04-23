#include "arenatracker.h"

ArenaTracker::ArenaTracker(int& argc, char **argv)
    : QApplication(argc, argv)
{
    setupApp();
    preferences = new Preferences();
    trayIcon = new TrayIcon(this);
}

ArenaTracker::~ArenaTracker()
{
    if(preferences){
        delete preferences;
        preferences = NULL;
    }
    if(trayIcon){
        delete trayIcon;
        trayIcon = NULL;
    }
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
  setOrganizationName("edipo.com");
  setOrganizationDomain("edipo.com");
  setWindowIcon(icon);
}

int ArenaTracker::run()
{
    return exec();
}

void ArenaTracker::showPreferences()
{
    preferences->show();
}
