#include "arenatracker.h"

ArenaTracker::ArenaTracker(int& argc, char **argv)
    : QApplication(argc, argv)
{
    settings = new Settings();
    setup();
}

ArenaTracker::~ArenaTracker()
{
    if(settings){
        delete settings;
        settings = NULL;
    }
}

void ArenaTracker::setup()
{
#if defined Q_OS_MAC
  setAttribute(Qt::AA_UseHighDpiPixmaps);
  QIcon icon(":/res/icon_black.png");
  icon.addFile(":/res/icon_black@2x.png");
#elif defined Q_OS_WIN
  QIcon icon(":/res/icon.ico");
#endif
  setApplicationName("Arena Tracker"); // for proper DataLocation handling
  setOrganizationName("edipo.com");
  setOrganizationDomain("edipo.com");
  setWindowIcon(icon);
}

int ArenaTracker::run()
{
    settings->show();
    return exec();
}
