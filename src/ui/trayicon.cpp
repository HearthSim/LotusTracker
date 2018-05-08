#include "trayicon.h"
#include "../arenatracker.h"
#include "../macros.h"
#include "../mtg/mtgalogparser.h"
#include "../mtg/mtgcards.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QMenu>

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        setupTrayIcon();
    }
}

TrayIcon::~TrayIcon()
{
    DEL(trayIcon);
}

void TrayIcon::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &TrayIcon::TrayIconActivated);
    QMenu *trayMenu = new QMenu();
    trayIcon->setContextMenu(trayMenu);

#if defined Q_OS_MAC
    QIcon icon;
    icon.addFile(":/res/icons/icon_black.png");
    icon.addFile(":/res/icons/icon_black@2x.png");
    icon.setIsMask(true);
#elif defined Q_OS_WIN
    QIcon icon = QIcon(":/res/icons/icon.ico");
#endif
    trayIcon->setIcon(icon);

    QAction *settingsAction = new QAction(tr("Preferences"), this);
    connect(settingsAction, &QAction::triggered, this, &TrayIcon::openPreferences);
    trayMenu->addAction(settingsAction);
#ifdef QT_DEBUG
    QMenu* testMenu = trayMenu->addMenu(tr("Tests"));
    configTestMenu(testMenu);
#endif
    QAction *quitAction = new QAction(tr("Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayMenu->addAction(quitAction);

    trayIcon->show();
}

void TrayIcon::TrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
#ifdef Q_OS_WIN
    if (reason == QSystemTrayIcon::ActivationReason::DoubleClick) {
        openPreferences();
    }
#else
    UNUSED(reason)
#endif
}

void TrayIcon::openPreferences()
{
    ArenaTracker *arenaTracker = (ArenaTracker*) qApp->instance();
    arenaTracker->showPreferences();
}

void TrayIcon::configTestMenu(QMenu* testMenu)
{
    // Load Deck
    QAction *loadDeckAction = new QAction(tr("Load Deck"), this);
    connect(loadDeckAction, &QAction::triggered, this, [this](){
        MtgaLogParser *mtgaLogParser = ARENA_TRACKER->mtgArena->getLogParser();
        emit mtgaLogParser->sgnMatchCreated(Match("Opponent", "Beginner", 0));
        // Player Select Deck
        QString currentDir = QDir::currentPath();
#ifdef Q_OS_MAC
        currentDir = currentDir.left(currentDir.indexOf(".app"));
        currentDir = currentDir.left(currentDir.lastIndexOf(QDir::separator()));
#endif
        QFile *logFile = new QFile(currentDir + QDir::separator() + "PlayerDeckSelected.txt");
        if(logFile->open(QFile::ReadOnly | QFile::Text)) {
            QString logContent = QTextStream(logFile).readAll();
            mtgaLogParser->parse(logContent);
        } else {
            LOGW("PlayerDeckSelected.txt file not found in current dir");
        }
    });
    testMenu->addAction(loadDeckAction);
    // Player Draw card
    QAction *playerDrawAction = new QAction(tr("Player Draw"), this);
    connect(playerDrawAction, &QAction::triggered, this, [this](){
        Card* card = ARENA_TRACKER->mtgCards->findCard(66825);
        MtgaLogParser *mtgaLogParser = ARENA_TRACKER->mtgArena->getLogParser();
        emit mtgaLogParser->sgnPlayerDrawCard(card);
    });
    testMenu->addAction(playerDrawAction);
    // Opponent play card
    QAction *opponentPlayAction = new QAction(tr("Opponent Play"), this);
    connect(opponentPlayAction, &QAction::triggered, this, [this](){
        int randomCardNumber = rand() % 200;
        Card* card = ARENA_TRACKER->mtgCards->findCard(66619 + randomCardNumber * 2);
        MtgaLogParser *mtgaLogParser = ARENA_TRACKER->mtgArena->getLogParser();
        emit mtgaLogParser->sgnOpponentPlayCard(card);
    });
    testMenu->addAction(opponentPlayAction);
}
