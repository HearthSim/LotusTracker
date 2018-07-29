#include "arenatracker.h"
#include "macros.h"
#include "mtg/mtgalogparser.h"
#include "utils/cocoainitializer.h"

#if defined Q_OS_MAC
#include "updater/macsparkleupdater.h"
#elif defined Q_OS_WIN
#include "updater/winsparkleupdater.h"
#endif

#include <QLocalSocket>
#include <QMessageBox>

ArenaTracker::ArenaTracker(int& argc, char **argv): QApplication(argc, argv)
{
    setupApp();
    setupUpdater();
    logger = new Logger(this);
    appSettings = new AppSettings(this);
    mtgCards = new MtgCards(this);
    mtgArena = new MtgArena(this, mtgCards);
    deckTrackerPlayer = new DeckTrackerPlayer();
    deckTrackerOpponent = new DeckTrackerOpponent();
    trayIcon = new TrayIcon(this, mtgCards);
    firebaseAuth = new FirebaseAuth(this);
    firebaseDatabase = new FirebaseDatabase(this, firebaseAuth);
    startScreen = new StartScreen(nullptr, firebaseAuth);
    connect(firebaseAuth, &FirebaseAuth::sgnUserLogged, this, &ArenaTracker::onUserSigned);
    connect(firebaseAuth, &FirebaseAuth::sgnTokenRefreshed, this, &ArenaTracker::onUserTokenRefreshed);
    connect(firebaseAuth, &FirebaseAuth::sgnTokenRefreshError, this, &ArenaTracker::onUserTokenRefreshError);
    //setupMatch should be called before setupLogParser because sgnMatchInfoResult order
    setupMtgaMatch();
    setupLogParserConnections();
    setupPreferencesScreen();
    checkForAutoLogin();
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
    DEL(firebaseAuth)
    DEL(firebaseDatabase)
}

int ArenaTracker::run()
{
    return isAlreadyRunning() ? 1 : exec();
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
  setAttribute(Qt::AA_Use96Dpi);
  setApplicationName("Arena Tracker");
  setOrganizationName("ArenaMeta");
  setOrganizationDomain("arenameta.com");
  setWindowIcon(icon);
}

void ArenaTracker::setupUpdater()
{
#if defined Q_OS_MAC
  CocoaInitializer cocoaInitializer;
  sparkleUpdater = new MacSparkleUpdater("https://uc8f198890f22917ff34d66e34ac.dl.dropboxusercontent.com/cd/0/get/ALc1ZFHRTaa-SohnuBN37QPilBjKcPn_TY2LSmLZKgPwxmgCRKjFRIfLz9spKqIbd5RYDwUhCWpd5-9TxWyhRPGI6M23le8aEZxoSy0DPsrHgTTUq-sM4AihqT1yMPttrbruYV36ZjUM1b5bZItqeeJXN8qOZLaj-OEUcs3VGTKxtVJzwnXOw6skc-kJVZq_9O0/file?dl=1");
#elif defined Q_OS_WIN
  sparkleUpdater = new WinSparkleUpdater("https://uc8f198890f22917ff34d66e34ac.dl.dropboxusercontent.com/cd/0/get/ALc1ZFHRTaa-SohnuBN37QPilBjKcPn_TY2LSmLZKgPwxmgCRKjFRIfLz9spKqIbd5RYDwUhCWpd5-9TxWyhRPGI6M23le8aEZxoSy0DPsrHgTTUq-sM4AihqT1yMPttrbruYV36ZjUM1b5bZItqeeJXN8qOZLaj-OEUcs3VGTKxtVJzwnXOw6skc-kJVZq_9O0/file?dl=1");
#endif
}

bool ArenaTracker::isAlreadyRunning() {
    QString serverName = "ArenaTracker";
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        QMessageBox::information(preferencesScreen, "Arena Tracker",
                                 "Arena Tracker already running.", QMessageBox::Ok);
        return true;
    }
    QLocalServer::removeServer(serverName);
    localServer = new QLocalServer(NULL);
    if (!localServer->listen(serverName)) {
        return true;
    }
    return false;
}

void ArenaTracker::setupPreferencesScreen()
{
    preferencesScreen = new PreferencesScreen();
    // Deck tracker player
    connect(preferencesScreen, &PreferencesScreen::sgnPlayerTrackerEnabled,
            this, &ArenaTracker::onDeckTrackerPlayerEnabledChange);
    connect(preferencesScreen, &PreferencesScreen::sgnTrackerAlpha,
            deckTrackerPlayer, &DeckTrackerBase::changeAlpha);
    connect(preferencesScreen, &PreferencesScreen::sgnUnhideDelay,
            deckTrackerPlayer, &DeckTrackerBase::changeUnhiddenTimeout);
    connect(preferencesScreen, &PreferencesScreen::sgnTrackerCardLayout,
            deckTrackerPlayer, &DeckTrackerBase::changeCardLayout);
    connect(preferencesScreen, &PreferencesScreen::sgnShowCardOnHoverEnabled,
            deckTrackerPlayer, &DeckTrackerBase::onShowCardOnHoverEnabled);
    connect(preferencesScreen, &PreferencesScreen::sgnShowOnlyRemainingCardsEnabled,
            deckTrackerPlayer, &DeckTrackerPlayer::onShowOnlyRemainingCardsEnabled);
    connect(preferencesScreen, &PreferencesScreen::sgnPlayerTrackerStatistics,
            deckTrackerPlayer, &DeckTrackerPlayer::onStatisticsEnabled);
    connect(preferencesScreen, &PreferencesScreen::sgnRestoreDefaults,
            deckTrackerPlayer, &DeckTrackerPlayer::applyCurrentSettings);
    // Deck tracker opponent
    connect(preferencesScreen, &PreferencesScreen::sgnOpponentTrackerEnabled,
            this, &ArenaTracker::onDeckTrackerOpponentEnabledChange);
    connect(preferencesScreen, &PreferencesScreen::sgnTrackerAlpha,
            deckTrackerOpponent, &DeckTrackerBase::changeAlpha);
    connect(preferencesScreen, &PreferencesScreen::sgnUnhideDelay,
            deckTrackerOpponent, &DeckTrackerBase::changeUnhiddenTimeout);
    connect(preferencesScreen, &PreferencesScreen::sgnTrackerCardLayout,
            deckTrackerOpponent, &DeckTrackerBase::changeCardLayout);
    connect(preferencesScreen, &PreferencesScreen::sgnShowCardOnHoverEnabled,
            deckTrackerOpponent, &DeckTrackerBase::onShowCardOnHoverEnabled);
    connect(preferencesScreen, &PreferencesScreen::sgnRestoreDefaults,
            deckTrackerOpponent, &DeckTrackerOpponent::applyCurrentSettings);
}

void ArenaTracker::setupLogParserConnections()
{
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerCollection,
            firebaseDatabase, &FirebaseDatabase::updatePlayerCollection);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerInventory,
            firebaseDatabase, &FirebaseDatabase::updateUserInventory);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerRankInfo,
            mtgaMatch, &MtgaMatch::onPlayerRankInfo);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckSubmited,
            this, &ArenaTracker::onDeckSubmited);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchCreated,
            this, &ArenaTracker::onMatchStart);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoResult,
            this, &ArenaTracker::onMatchEnd);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerTakesMulligan,
            this, &ArenaTracker::onPlayerTakesMulligan);
}

void ArenaTracker::setupMtgaMatch()
{
    mtgaMatch = new MtgaMatch(this, mtgCards);
    // Player
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutInLibraryCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerPutInLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDrawCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDrawCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDiscardCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDiscardCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDiscardFromLibraryCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDiscardFromLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutOnBattlefieldCard,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerPutOnBattlefieldCard);
    // Opponent
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutInLibraryCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPutInLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPlayCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPlayCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentDiscardCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentDiscardCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentDiscardFromLibraryCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentDiscardFromLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutOnBattlefieldCard,
            deckTrackerOpponent, &DeckTrackerOpponent::onOpponentPutOnBattlefieldCard);
    // Match
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchCreated,
            mtgaMatch, &MtgaMatch::onStartNewMatch);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoResult,
            mtgaMatch, &MtgaMatch::onEndCurrentMatch);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoSeats,
            mtgaMatch, &MtgaMatch::onMatchInfoSeats);
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

void ArenaTracker::avoidAppClose()
{
    preferencesScreen->show();
    preferencesScreen->hide();
}

void ArenaTracker::showStartScreen()
{
    startScreen->show();
    startScreen->raise();
}

void ArenaTracker::showPreferencesScreen()
{
    preferencesScreen->show();
    preferencesScreen->raise();
}

void ArenaTracker::showMessage(QString msg, QString title)
{
    trayIcon->showMessage(title, msg);
}

void ArenaTracker::onDeckSubmited(Deck deck)
{
    deckTrackerPlayer->loadDeck(deck);
    firebaseDatabase->updatePlayerDeck(deck);
}

void ArenaTracker::onMatchStart(QString eventId, OpponentInfo opponentInfo)
{
    UNUSED(eventId);
    UNUSED(opponentInfo);
    if (APP_SETTINGS->isDeckTrackerPlayerEnabled()) {
        deckTrackerPlayer->show();
    }
    if (APP_SETTINGS->isDeckTrackerOpponentEnabled()) {
        deckTrackerOpponent->show();
    }
}

void ArenaTracker::onMatchEnd(int winningTeamId)
{
    UNUSED(winningTeamId);
    firebaseDatabase->uploadMatch(mtgaMatch->getInfo(),
                                  mtgaMatch->getPlayerRankInfo().first,
                                  deckTrackerPlayer->getDeck(),
                                  deckTrackerOpponent->getDeck());
    deckTrackerPlayer->hide();
    deckTrackerPlayer->resetDeck();
    deckTrackerOpponent->hide();
    deckTrackerOpponent->clearDeck();
}

void ArenaTracker::onPlayerTakesMulligan()
{
    deckTrackerPlayer->resetDeck();
}

void ArenaTracker::onDeckTrackerPlayerEnabledChange(bool enabled)
{
    if (enabled && mtgaMatch->isRunning) {
        deckTrackerPlayer->show();
    }
    if (!enabled && mtgaMatch->isRunning) {
        deckTrackerPlayer->hide();
    }
}

void ArenaTracker::onDeckTrackerOpponentEnabledChange(bool enabled)
{
    if (enabled && mtgaMatch->isRunning) {
        deckTrackerOpponent->show();
    }
    if (!enabled && mtgaMatch->isRunning) {
        deckTrackerOpponent->hide();
    }
}

void ArenaTracker::checkForAutoLogin()
{
    UserSettings userSettings = appSettings->getUserSettings();
    switch (userSettings.getAuthStatus()) {
        case AUTH_VALID: {
            emit firebaseAuth->sgnUserLogged(false);
            break;
        }
        case AUTH_EXPIRED: {
            firebaseAuth->refreshToken(userSettings.refreshToken);
            break;
        }
        case AUTH_INVALID: {
            startScreen->show();
            startScreen->raise();
            break;
        }
    }
}

void ArenaTracker::onUserSigned(bool fromSignUp)
{
    UNUSED(fromSignUp);
    startScreen->hide();
    trayIcon->updateUserSettings();
}

void ArenaTracker::onUserTokenRefreshed()
{
    startScreen->hide();
    trayIcon->updateUserSettings();
}

void ArenaTracker::onUserTokenRefreshError()
{
    startScreen->show();
    trayIcon->updateUserSettings();
}
