#include "trayicon.h"
#include "../lotustracker.h"
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
    trayIcon->setToolTip(QString("%1 - %2").arg(qApp->applicationName())
                         .arg(qApp->applicationVersion()));
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
    UserSettings userSettings = LOTUS_TRACKER->appSettings->getUserSettings();
    bool isAuthValid = userSettings.getAuthStatus() == AUTH_VALID;
    QString userText = QString("%1 (Logout)").arg(userSettings.getUserName());
    signAction->setText(isAuthValid ? userText : tr("Sign In"));
}

void TrayIcon::openSignInOrSignOut()
{
    UserSettings userSettings = LOTUS_TRACKER->appSettings->getUserSettings();
    bool isAuthValid = userSettings.getAuthStatus() == AUTH_VALID;
    if (isAuthValid) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Logout user"), tr("Are you sure?"),
                           QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.deleteLater();
        if (msgBox.exec() == QMessageBox::Yes) {
            LOTUS_TRACKER->appSettings->clearUserSettings();
            LOTUS_TRACKER->showStartScreen();
            updateUserSettings();
        }
        LOTUS_TRACKER->avoidAppClose();
    } else {
        LOTUS_TRACKER->showStartScreen();
    }
}

void TrayIcon::openPreferences()
{
    LOTUS_TRACKER->showPreferencesScreen();
}

void TrayIcon::configTestMenu(QMenu* testMenu)
{
    // Load Deck
    QAction *loadDeckAction = new QAction(tr("Load Deck"), this);
    connect(loadDeckAction, &QAction::triggered, this, [](){
        MtgaLogParser *mtgaLogParser = LOTUS_TRACKER->mtgArena->getLogParser();
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
            emit mtgaLogParser->sgnGameStart(MatchMode_SINGLE, {{}}, 1);
        } else {
            LOGW("PlayerDeckSubmit.txt file not found in current dir");
        }
    });
    testMenu->addAction(loadDeckAction);
    // Player Draw card
    QAction *playerDrawAction = new QAction(tr("Player Draw"), this);
    connect(playerDrawAction, &QAction::triggered, this, [](){
        Card* card = LOTUS_TRACKER->mtgCards->findCard(68186);
        emit LOTUS_TRACKER->mtgaMatch->sgnPlayerDrawCard(card);
    });
    testMenu->addAction(playerDrawAction);
    // Opponent play card
    QAction *opponentPlayAction = new QAction(tr("Opponent Play"), this);
    connect(opponentPlayAction, &QAction::triggered, this, [](){
        srand(static_cast<unsigned int>(QTime::currentTime().msec()));
        int randomCardNumber = rand() % 200;
        Card* card = LOTUS_TRACKER->mtgCards->findCard(66619 + randomCardNumber * 2);
        emit LOTUS_TRACKER->mtgaMatch->sgnOpponentPlayCard(card);
    });
    testMenu->addAction(opponentPlayAction);
    // Test opponent deck arch
    QAction *opponentDeckArchAction = new QAction(tr("Opponent Deck Arch"), this);
    connect(opponentDeckArchAction, &QAction::triggered, this, [](){
        QList<int> cardsMtgaId = {64129, 65343, 65343, 65993, 67238, 67586};
        for (int cardMtgaId : cardsMtgaId) {
            Card* card = LOTUS_TRACKER->mtgCards->findCard(cardMtgaId);
            emit LOTUS_TRACKER->mtgaMatch->sgnOpponentPlayCard(card);
        }
    });
    testMenu->addAction(opponentDeckArchAction);
    // Update user inventory
    QAction *updateUserInventoryAction = new QAction(tr("parse UserInventory"), this);
    connect(updateUserInventoryAction, &QAction::triggered, this, [](){
        PlayerInventory playerInventory(rand() % 10, rand() % 10, rand() % 10, rand() % 10);
        emit LOTUS_TRACKER->mtgArena->getLogParser()->sgnPlayerInventory(playerInventory);
    });
    testMenu->addAction(updateUserInventoryAction);
}
