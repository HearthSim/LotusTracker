#include "trayicon.h"
#include "../arenatracker.h"
#include "../macros.h"
#include "../mtg/mtgalogparser.h"
#include "../mtg/mtgcards.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QMessageBox>

TrayIcon::TrayIcon(QObject *parent): QObject(parent)
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

    signAction = new QAction(tr("Sign In"), this);
    connect(signAction, &QAction::triggered, this, &TrayIcon::openSignInOrSignOut);
    trayMenu->addAction(signAction);
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
    UNUSED(reason);
#endif
}

void TrayIcon::showMessage(QString title, QString msg)
{
    trayIcon->showMessage(title, msg);
}

void TrayIcon::updateUserSettings()
{
    UserSettings userSettings = ARENA_TRACKER->appSettings->getUserSettings();
    bool isAuthValid = userSettings.getAuthStatus() == AUTH_VALID;
    QString userText = QString("%1 (Logout)").arg(userSettings.getUserName());
    signAction->setText(isAuthValid ? userText : tr("Sign In"));
}

void TrayIcon::openSignInOrSignOut()
{
    UserSettings userSettings = ARENA_TRACKER->appSettings->getUserSettings();
    bool isAuthValid = userSettings.getAuthStatus() == AUTH_VALID;
    if (isAuthValid) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Logout user"), tr("Are you sure?"),
                           QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.deleteLater();
        if (msgBox.exec() == QMessageBox::Yes) {
            ARENA_TRACKER->appSettings->clearUserSettings();
            ARENA_TRACKER->showStartScreen();
            updateUserSettings();
        }
        ARENA_TRACKER->avoidAppClose();
    } else {
        ARENA_TRACKER->showStartScreen();
    }
}

void TrayIcon::openPreferences()
{
    ArenaTracker *arenaTracker = (ArenaTracker*) qApp->instance();
    arenaTracker->showPreferencesScreen();
}

void TrayIcon::configTestMenu(QMenu* testMenu)
{
    // Load Deck
    QAction *loadDeckAction = new QAction(tr("Load Deck"), this);
    connect(loadDeckAction, &QAction::triggered, this, [](){
        MtgaLogParser *mtgaLogParser = ARENA_TRACKER->mtgArena->getLogParser();
        emit mtgaLogParser->sgnMatchCreated(QString("ConstructedRanked1"),
                                            OpponentInfo("", "Beginner", 0));
        // Player Select Deck
        QString currentDir = QDir::currentPath();
#ifdef Q_OS_MAC
        currentDir = currentDir.left(currentDir.indexOf(".app"));
        currentDir = currentDir.left(currentDir.lastIndexOf(QDir::separator()));
#endif
        QFile *logFile = new QFile(currentDir + QDir::separator() + "PlayerDeckSubmit.txt");
        if(logFile->open(QFile::ReadOnly | QFile::Text)) {
            QString logContent = QTextStream(logFile).readAll();
            mtgaLogParser->parse(logContent);
        } else {
            LOGW("PlayerDeckSubmit.txt file not found in current dir");
        }
    });
    testMenu->addAction(loadDeckAction);
    // Player Draw card
    QAction *playerDrawAction = new QAction(tr("Player Draw"), this);
    connect(playerDrawAction, &QAction::triggered, this, [this](){
        Card* card = ARENA_TRACKER->mtgCards->findCard(68186);
        emit ARENA_TRACKER->mtgaMatch->sgnPlayerDrawCard(card);
    });
    testMenu->addAction(playerDrawAction);
    // Opponent play card
    QAction *opponentPlayAction = new QAction(tr("Opponent Play"), this);
    connect(opponentPlayAction, &QAction::triggered, this, [this](){
        srand(QTime::currentTime().msec());
        int randomCardNumber = rand() % 200;
        Card* card = ARENA_TRACKER->mtgCards->findCard(66619 + randomCardNumber * 2);
        emit ARENA_TRACKER->mtgaMatch->sgnOpponentPlayCard(card);
    });
    testMenu->addAction(opponentPlayAction);
    // Update user inventory
    QAction *updateUserInventoryAction = new QAction(tr("parse UserInventory"), this);
    connect(updateUserInventoryAction, &QAction::triggered, this, [this](){
        PlayerInventory playerInventory(rand() % 10, rand() % 10, rand() % 10, rand() % 10);
        emit ARENA_TRACKER->mtgArena->getLogParser()->sgnPlayerInventory(playerInventory);
    });
    testMenu->addAction(updateUserInventoryAction);
}
