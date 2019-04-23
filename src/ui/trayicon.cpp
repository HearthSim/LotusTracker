#include "trayicon.h"
#include "../lotustracker.h"
#include "../macros.h"
#include "../urls.h"
#include "../mtg/mtgalogparser.h"
#include "../mtg/mtgcards.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include <QDesktopServices>

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
    connect(signAction, &QAction::triggered, this, &TrayIcon::signIn);
    trayMenu->addAction(signAction);
    profileAction = new QAction(tr("My Profile"), this);
    connect(profileAction, &QAction::triggered, this, &TrayIcon::openProfile);
    trayMenu->addAction(profileAction);
    profileAction->setVisible(false);
    QAction *settingsAction = new QAction(tr("Preferences"), this);
    connect(settingsAction, &QAction::triggered, this, &TrayIcon::openPreferences);
    trayMenu->addAction(settingsAction);
#ifdef QT_DEBUG
    QMenu* testMenu = trayMenu->addMenu(tr("Tests"));
    configTestMenu(testMenu);
#endif
    logoutAction = new QAction(tr("Logout"), this);
    connect(logoutAction, &QAction::triggered, this, &TrayIcon::signOut);
    trayMenu->addAction(logoutAction);
    logoutAction->setVisible(false);
    QAction *quitAction = new QAction(tr("Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, []() {
        LOTUS_TRACKER->gaTracker->sendEvent("LotusTracker", "Quit");
        QCoreApplication::quit();
    });
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
    signAction->setVisible(!isAuthValid);
    profileAction->setVisible(isAuthValid);
    logoutAction->setVisible(isAuthValid);
    LOTUS_TRACKER->gaTracker->setUserID(userSettings.userId);
}

void TrayIcon::signIn()
{
    LOTUS_TRACKER->showStartScreen();
}

void TrayIcon::openProfile()
{
    UserSettings userSettings = APP_SETTINGS->getUserSettings();
    QString link = QString("%1/user?localId=%2&email=%3&idToken=%4&refreshToken=%5&userName=%6&expiresIn=%7")
            .arg(URLs::SITE()).arg(userSettings.userId).arg(userSettings.userEmail).arg(userSettings.userToken)
            .arg(userSettings.refreshToken).arg(userSettings.getUserName()).arg(userSettings.expiresTokenEpoch);
    QDesktopServices::openUrl(QUrl(link));
     LOTUS_TRACKER->gaTracker->sendEvent("TrayIcon", "User Profile");
}

void TrayIcon::openPreferences()
{
    LOTUS_TRACKER->showPreferencesScreen();
    LOTUS_TRACKER->gaTracker->sendEvent("TrayIcon", "Preferences");
}

void TrayIcon::signOut()
{
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
    LOTUS_TRACKER->gaTracker->sendEvent("Tracker", "Logout");
}

void TrayIcon::configTestMenu(QMenu* testMenu)
{
    // Load Deck
    QAction *loadDeckAction = new QAction(tr("Load Deck"), this);
    connect(loadDeckAction, &QAction::triggered, this, [this](){
        MtgaLogParser *mtgaLogParser = LOTUS_TRACKER->mtgArena->getLogParser();
        emit mtgaLogParser->sgnMatchCreated(QString("ConstructedRanked1"),
                                            OpponentInfo("", "Beginner", 0));
        // Player Select Deck
        QString applicationDir = QCoreApplication::applicationDirPath();
#if defined Q_OS_MAC
        applicationDir = currentDir.replace("/Contents/MacOS", "");
        applicationDir = currentDir.left(currentDir.lastIndexOf("/"));
#endif
        QFile *logFile = new QFile(applicationDir + QDir::separator() + "PlayerDeckSubmit.txt");
        if(logFile->open(QFile::ReadOnly | QFile::Text)) {
            QString logContent = QTextStream(logFile).readAll();
            mtgaLogParser->parse(logContent);
            emit sgnShowDeckOverlay();
        } else {
            LOGW(QString("PlayerDeckSubmit.txt file not found in current dir: %1").arg(logFile->fileName()));
        }
    });
    testMenu->addAction(loadDeckAction);
    // Load Sideboard
    QAction *loadSideboardAction = new QAction(tr("Load Sideboard"), this);
    connect(loadSideboardAction, &QAction::triggered, this, [this](){
        MtgaLogParser *mtgaLogParser = LOTUS_TRACKER->mtgArena->getLogParser();
        // Player Select Deck
        QString applicationDir = QCoreApplication::applicationDirPath();
#if defined Q_OS_MAC
        applicationDir = currentDir.replace("/Contents/MacOS", "");
        applicationDir = currentDir.left(currentDir.lastIndexOf("/"));
#endif
        QFile *logFile = new QFile(applicationDir + QDir::separator() + "PlayerDeckSideboard.txt");
        if(logFile->open(QFile::ReadOnly | QFile::Text)) {
            QString logContent = QTextStream(logFile).readAll();
            mtgaLogParser->parse(logContent);
            emit sgnShowDeckOverlay();
        } else {
            LOGW("PlayerDeckSideboard.txt file not found in current dir");
        }
    });
    testMenu->addAction(loadSideboardAction);
    // Player Draw card
    QAction *playerDrawAction = new QAction(tr("Player Draw"), this);
    connect(playerDrawAction, &QAction::triggered, this, [](){
        Card* card = LOTUS_TRACKER->mtgCards->findCard(66829);
        emit LOTUS_TRACKER->mtgaMatch->sgnPlayerDrawCard(card);
    });
    testMenu->addAction(playerDrawAction);
    // Opponent play card
    QAction *opponentPlayAction = new QAction(tr("Opponent Play"), this);
    connect(opponentPlayAction, &QAction::triggered, this, [this](){
        srand(static_cast<unsigned int>(QTime::currentTime().msec()));
        int randomCardNumber = rand() % 200;
        Card* card = LOTUS_TRACKER->mtgCards->findCard(66619 + randomCardNumber * 2);
        emit LOTUS_TRACKER->mtgaMatch->sgnOpponentPlayCard(card);
        emit sgnShowDeckOverlay();
    });
    testMenu->addAction(opponentPlayAction);
    // Test opponent deck arch
    QAction *opponentDeckArchAction = new QAction(tr("Opponent Deck Arch"), this);
    connect(opponentDeckArchAction, &QAction::triggered, this, [this](){
        emit LOTUS_TRACKER->setEventInfo("Test", "Constructed");
        QList<int> cardsMtgaId = {66109, 68667, 68628, 66223, 67238, 66109};
        for (int cardMtgaId : cardsMtgaId) {
            Card* card = LOTUS_TRACKER->mtgCards->findCard(cardMtgaId);
            emit LOTUS_TRACKER->mtgaMatch->sgnOpponentPlayCard(card);
        }
        emit sgnShowDeckOverlay();
    });
    testMenu->addAction(opponentDeckArchAction);
    // Test deck overlay draft
    QAction *deckOverlayDraftAction = new QAction(tr("Parser Draft Status"), this);
    connect(deckOverlayDraftAction, &QAction::triggered, this, [](){
        QList<int> cardsMtgaId = { 66997, 66717, 66867, 66893, 66827, 66895, 66631, 66663, 66747, 66999, 66811, 67007, 66787, 66921 };
        QList<Card*> availableCards;
        QList<Card*> pickedCards;
        for (int mtgaId : cardsMtgaId) {
            Card* card = LOTUS_TRACKER->mtgCards->findCard(mtgaId);
            availableCards << card;
        }
        MtgaLogParser* logParser = LOTUS_TRACKER->mtgArena->getLogParser();
        emit logParser->sgnDraftStatus("QuickDraft_GRN_20190412", "Draft.PickNext", 1, 2, availableCards, pickedCards);
    });
    testMenu->addAction(deckOverlayDraftAction);
    // Test deck overlay draft make pick
    QAction *deckOverlayDraftPickAction = new QAction(tr("Parser Draft Pick"), this);
    connect(deckOverlayDraftPickAction, &QAction::triggered, this, [](){
        MtgaLogParser* logParser = LOTUS_TRACKER->mtgArena->getLogParser();
        emit logParser->sgnDraftPick(66717, 1, 2);
    });
    testMenu->addAction(deckOverlayDraftPickAction);
    // Update user inventory
    QAction *updateUserInventoryAction = new QAction(tr("parse UserInventory"), this);
    connect(updateUserInventoryAction, &QAction::triggered, this, [](){
        PlayerInventory playerInventory(rand() % 10, rand() % 10, rand() % 10, rand() % 10);
        MtgaLogParser* logParser = LOTUS_TRACKER->mtgArena->getLogParser();
        emit logParser->sgnPlayerInventory(playerInventory);
    });
    testMenu->addAction(updateUserInventoryAction);
}
