#include "lotustracker.h"
#include "macros.h"
#include "urls.h"
#include "mtg/mtgalogparser.h"
#include "utils/cocoainitializer.h"

#if defined Q_OS_MAC
#include "updater/macsparkleupdater.h"
#elif defined Q_OS_WIN
#include "updater/winsparkleupdater.h"
#endif

#include <QLocalSocket>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>

LotusTracker::LotusTracker(int& argc, char **argv): QApplication(argc, argv)
{
    setupApp();
    setupUpdater();
    logger = new Logger(this);
    appSettings = new AppSettings(this);
    mtgCards = new MtgCards(this);
    mtgDecksArch = new MtgDecksArch(this);
    mtgArena = new MtgArena(this);
    deckTrackerPlayer = new DeckTrackerPlayer();
    deckTrackerOpponent = new DeckTrackerOpponent();
    trayIcon = new TrayIcon(this);
    lotusAPI = new LotusTrackerAPI(this);
    startScreen = new StartScreen(nullptr, lotusAPI);
    hideTrackerTimer = new QTimer(this);
    mtgaMatch = new MtgaMatch(this, mtgCards);
    gaTracker = new GAnalytics(CREDENTIALS::GA_ID());
    connect(mtgArena, &MtgArena::sgnMTGAStarted,
            this, &LotusTracker::onGameStarted);
    connect(mtgArena, &MtgArena::sgnMTGAFocusChanged,
            this, &LotusTracker::onGameFocusChanged);
    connect(mtgArena, &MtgArena::sgnMTGAStopped,
            this, &LotusTracker::onGameStopped);
    connect(lotusAPI, &LotusTrackerAPI::sgnDeckWinRate,
            deckTrackerPlayer, &DeckTrackerPlayer::onPlayerDeckStatus);
    connect(lotusAPI, &LotusTrackerAPI::sgnEventInfo,
            deckTrackerPlayer, &DeckTrackerPlayer::onReceiveEventInfo);
    connect(lotusAPI, &LotusTrackerAPI::sgnRequestFinishedWithSuccess,
            deckTrackerPlayer, &DeckTrackerPlayer::onLotusAPIRequestFinishedWithSuccess);
    connect(lotusAPI, &LotusTrackerAPI::sgnRequestFinishedWithError,
            deckTrackerPlayer, &DeckTrackerPlayer::onLotusAPIRequestFinishedWithError);
    connect(lotusAPI, &LotusTrackerAPI::sgnEventInfo,
            deckTrackerOpponent, &DeckTrackerOpponent::onReceiveEventInfo);
    connect(lotusAPI, &LotusTrackerAPI::sgnUserLogged,
            this, &LotusTracker::onUserSigned);
    connect(lotusAPI, &LotusTrackerAPI::sgnTokenRefreshed,
            this, &LotusTracker::onUserTokenRefreshed);
    connect(lotusAPI, &LotusTrackerAPI::sgnTokenRefreshError,
            this, &LotusTracker::onUserTokenRefreshError);
    connect(hideTrackerTimer, &QTimer::timeout, this, [this]{
        hideTrackerTimer->stop();
        deckTrackerPlayer->reset();
        deckTrackerPlayer->hide();
        deckTrackerOpponent->reset();
        deckTrackerOpponent->hide();
    });
    //setupMatch should be called before setupLogParser because sgnMatchInfoResult order
    setupLogParserConnections();
    setupMtgaMatchConnections();
    setupPreferencesScreen();
    LOGI("Lotus Tracker started");
    if (APP_SETTINGS->isFirstRun()) {
        startScreen->show();
        startScreen->raise();
        showMessage(tr("Lotus Tracker is running in background, you can click on tray icon for preferences."));
    } else {
        checkConnection = new QTimer();
        connect(checkConnection, &QTimer::timeout, this, [this]() {
            LOGD("Checking internet connection..");
            if (isOnline()) {
                LOGD("Internet connection OK");
                checkConnection->stop();
                checkForAutoLogin();
            }
        });
        checkConnection->start(3000);
    }
}

LotusTracker::~LotusTracker()
{
    DEL(logger)
    DEL(deckTrackerPlayer)
    DEL(deckTrackerOpponent)
    DEL(preferencesScreen)
    DEL(trayIcon)
    DEL(mtgArena)
    DEL(mtgaMatch)
    DEL(lotusAPI)
    DEL(gaTracker)
}

int LotusTracker::run()
{
    try {
        return isAlreadyRunning() ? 1 : exec();
    } catch (const std::exception& ex) {
        gaTracker->sendException(ex.what());
        return -1;
    }
}

void LotusTracker::setupApp()
{
#if defined Q_OS_MAC
  setAttribute(Qt::AA_UseHighDpiPixmaps);
  QIcon icon(":/res/icon_black.png");
  icon.addFile(":/res/icon_black@2x.png");
#elif defined Q_OS_WIN
  QIcon icon(":/res/icon.ico");
#endif
  setAttribute(Qt::AA_Use96Dpi);
  setApplicationName("Lotus Tracker");
  setApplicationVersion(VERSION);
  setOrganizationName("Black Lotus Valley");
  setOrganizationDomain("blacklotusvalley.com");
  setWindowIcon(icon);
}

void LotusTracker::setupUpdater()
{
#if defined Q_OS_MAC
    CocoaInitializer cocoaInitializer;
    QString updateUrl = QString("%1/%2").arg(Server::URL()).arg("appcast-osx.xml");
    sparkleUpdater = new MacSparkleUpdater(updateUrl);
#elif defined Q_OS_WIN
    QString updateUrl = QString("%1/%2").arg(URLs::SITE()).arg("appcast-win.xml");
    sparkleUpdater = new WinSparkleUpdater(updateUrl);
#endif
}

bool LotusTracker::isAlreadyRunning() {
    QString serverName = "ArenaTracker";
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        QMessageBox::information(preferencesScreen, "Lotus Tracker",
                                 "Lotus Tracker already running in background.", QMessageBox::Ok);
        return true;
    }
    QLocalServer::removeServer(serverName);
    localServer = new QLocalServer(nullptr);
    if (!localServer->listen(serverName)) {
        return true;
    }
    return false;
}

bool LotusTracker::isOnline()
{
    QNetworkAccessManager nam;
    QNetworkRequest req(QUrl("http://www.google.com"));
    QNetworkReply *reply = nam.get(req);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    return reply->bytesAvailable();
}

void LotusTracker::setupPreferencesScreen()
{
    preferencesScreen = new PreferencesScreen();
    // Tab General
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnRestoreDefaults,
            preferencesScreen->getTabOverlay(), &TabOverlay::onRestoreDefaultsSettings);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnPlayerTrackerEnabled,
            this, &LotusTracker::onDeckTrackerPlayerEnabledChange);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnRestoreDefaults,
            deckTrackerPlayer, &DeckTrackerPlayer::applyCurrentSettings);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnOpponentTrackerEnabled,
            this, &LotusTracker::onDeckTrackerOpponentEnabledChange);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnRestoreDefaults,
            deckTrackerOpponent, &DeckTrackerOpponent::applyCurrentSettings);
    // Tab Overlay
    // --- Player UI
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerAlpha,
            deckTrackerPlayer, &DeckTrackerBase::changeAlpha);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnUnhideDelay,
            deckTrackerPlayer, &DeckTrackerBase::changeUnhiddenTimeout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerCardLayout,
            deckTrackerPlayer, &DeckTrackerBase::changeCardLayout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardOnHoverEnabled,
            deckTrackerPlayer, &DeckTrackerBase::onShowCardOnHoverEnabled);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowOnlyRemainingCardsEnabled,
            deckTrackerPlayer, &DeckTrackerPlayer::onShowOnlyRemainingCardsEnabled);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnPlayerTrackerStatistics,
            deckTrackerPlayer, &DeckTrackerPlayer::onStatisticsEnabled);
    // --- Opponent UI
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerAlpha,
            deckTrackerOpponent, &DeckTrackerBase::changeAlpha);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnUnhideDelay,
            deckTrackerOpponent, &DeckTrackerBase::changeUnhiddenTimeout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerCardLayout,
            deckTrackerOpponent, &DeckTrackerBase::changeCardLayout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardOnHoverEnabled,
            deckTrackerOpponent, &DeckTrackerBase::onShowCardOnHoverEnabled);
    // Tab Logs
    connect(logger, &Logger::sgnLog,
            preferencesScreen->getTabLogs(), &TabLogs::onNewLog);
}

void LotusTracker::setupLogParserConnections()
{
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerCollection,
            lotusAPI, &LotusTrackerAPI::updatePlayerCollection);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerInventory,
            lotusAPI, &LotusTrackerAPI::updatePlayerInventory);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckCreated,
            lotusAPI, &LotusTrackerAPI::createPlayerDeck);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckUpdated,
            lotusAPI, &LotusTrackerAPI::updatePlayerDeck);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerRankInfo,
            mtgaMatch, &MtgaMatch::onPlayerRankInfo);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckSubmited,
            this, &LotusTracker::onDeckSubmited);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckWithSideboardSubmited,
            deckTrackerPlayer, &DeckTrackerPlayer::loadDeckWithSideboard);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnEventPlayerCourse,
            this, &LotusTracker::onEventPlayerCourse);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchCreated,
            this, &LotusTracker::onMatchStart);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnGameStart,
            this, &LotusTracker::onGameStart);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnGameCompleted,
            this, &LotusTracker::onGameCompleted);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchResult,
            this, &LotusTracker::onMatchEnds);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnEventFinish,
            this, &LotusTracker::onEventFinish);
}

void LotusTracker::setupMtgaMatchConnections()
{
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
    connect(mtgaMatch, &MtgaMatch::sgnPlayerUserName,
            lotusAPI, &LotusTrackerAPI::setPlayerUserName);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchInfoSeats,
            mtgaMatch, &MtgaMatch::onMatchInfoSeats);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMatchStateDiff,
            mtgaMatch, &MtgaMatch::onMatchStateDiff);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnNewTurnStarted,
            mtgaMatch, &MtgaMatch::onNewTurnStarted);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerTakesMulligan,
            mtgaMatch, &MtgaMatch::onPlayerTakesMulligan);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnOpponentTakesMulligan,
            mtgaMatch, &MtgaMatch::onOpponentTakesMulligan);
}

void LotusTracker::avoidAppClose()
{
    preferencesScreen->show();
    preferencesScreen->hide();
}

void LotusTracker::showStartScreen()
{
    startScreen->show();
    startScreen->raise();
}

void LotusTracker::showPreferencesScreen()
{
    preferencesScreen->show();
    preferencesScreen->raise();
}

void LotusTracker::showMessage(QString msg, QString title)
{
    trayIcon->showMessage(title, msg);
}

void LotusTracker::publishOrUpdatePlayerDeck(Deck deck)
{
    QString playerName = mtgaMatch->getPlayerName();
    lotusAPI->publishOrUpdatePlayerDeck(playerName, deck);
}

void LotusTracker::onDeckSubmited(QString eventId, Deck deck)
{
    deckTrackerPlayer->loadDeck(deck);
    lotusAPI->updatePlayerDeck(deck);
    lotusAPI->getMatchInfo(eventId, deck.id);
}

void LotusTracker::onEventPlayerCourse(QString eventId, Deck currentDeck)
{
    eventPlayerCourse = qMakePair(eventId, currentDeck);
}

void LotusTracker::onMatchStart(QString eventId, OpponentInfo opponentInfo)
{
    mtgaMatch->onStartNewMatch(eventId, opponentInfo);
    // Load deck from event in course if not loaded yet (event continues without submitDeck)
    if (eventId == eventPlayerCourse.first) {
        Deck deck = eventPlayerCourse.second;
        deckTrackerPlayer->loadDeck(deck);
        lotusAPI->getMatchInfo(eventId, deck.id);
    }
    deckTrackerOpponent->setEventId(eventId);
    gaTracker->sendEvent("Match", "starts", eventId);
}

void LotusTracker::onGameStart(MatchMode mode, QList<MatchZone> zones, int seatId)
{
    if (!mtgaMatch->isRunning) {
        return;
    }
    mtgaMatch->onGameStart(mode, zones, seatId);
    if (APP_SETTINGS->isDeckTrackerPlayerEnabled()) {
        deckTrackerPlayer->show();
    }
    if (APP_SETTINGS->isDeckTrackerOpponentEnabled()) {
        deckTrackerOpponent->show();
    }
    if (APP_SETTINGS->isFirstMatch()) {
        showMessage(tr("You can hide the tracker temporarily with a mouse right click."));
    }
}

void LotusTracker::onGameStarted()
{
    gaTracker->startSession();
}

void LotusTracker::onGameFocusChanged(bool hasFocus)
{
    if (!mtgaMatch->isRunning) {
        return;
    }
    if (APP_SETTINGS->isDeckTrackerPlayerEnabled()) {
        if (hasFocus) {
            deckTrackerPlayer->show();
        } else if (APP_SETTINGS->isHideOnLoseGameFocusEnabled()) {
            deckTrackerPlayer->hide();
        }
    }
    if (APP_SETTINGS->isDeckTrackerOpponentEnabled()) {
        if (hasFocus) {
            deckTrackerOpponent->show();
        } else if (APP_SETTINGS->isHideOnLoseGameFocusEnabled()) {
            deckTrackerOpponent->hide();
        }
    }
    gaTracker->sendEvent("Game", "Focus change");
}

void LotusTracker::onGameStopped()
{
    deckTrackerPlayer->hide();
    deckTrackerOpponent->hide();
    gaTracker->endSession();
}

void LotusTracker::onGameCompleted(QMap<int, int> teamIdWins)
{
    if (!mtgaMatch->isRunning) {
        return;
    }
    mtgaMatch->onGameCompleted(deckTrackerOpponent->getDeck(), teamIdWins);
    hideTrackerTimer->start(5000);
}

void LotusTracker::onMatchEnds(int winningTeamId)
{
    if (!mtgaMatch->isRunning) {
        return;
    }
    mtgaMatch->onEndCurrentMatch(winningTeamId);
    if (mtgaMatch->getInfo().eventId != "NPE"){
        lotusAPI->uploadMatch(mtgaMatch->getInfo(),
                              deckTrackerPlayer->getDeck(),
                              mtgaMatch->getPlayerRankInfo().first);
    }
    gaTracker->sendEvent("Match", "ends");
}

void LotusTracker::onEventFinish(QString eventId, QString deckId, QString deckColors,
                                 int maxWins, int wins, int losses)
{
    lotusAPI->uploadEventResult(eventId, deckId, deckColors, maxWins, wins, losses);
}

void LotusTracker::onDeckTrackerPlayerEnabledChange(bool enabled)
{
    if (enabled && mtgaMatch->isRunning) {
        deckTrackerPlayer->show();
    }
    if (!enabled && mtgaMatch->isRunning) {
        deckTrackerPlayer->hide();
    }
}

void LotusTracker::onDeckTrackerOpponentEnabledChange(bool enabled)
{
    if (enabled && mtgaMatch->isRunning) {
        deckTrackerOpponent->show();
    }
    if (!enabled && mtgaMatch->isRunning) {
        deckTrackerOpponent->hide();
    }
}

void LotusTracker::checkForAutoLogin()
{
    UserSettings userSettings = appSettings->getUserSettings();
    switch (userSettings.getAuthStatus()) {
        case AUTH_VALID: {
            emit lotusAPI->sgnUserLogged(false);
            break;
        }
        case AUTH_EXPIRED: {
            lotusAPI->refreshToken(userSettings.refreshToken);
            break;
        }
        case AUTH_INVALID: {
            break;
        }
    }
}

void LotusTracker::onUserSigned(bool fromSignUp)
{
    UNUSED(fromSignUp);
    startScreen->hide();
    trayIcon->updateUserSettings();
}

void LotusTracker::onUserTokenRefreshed()
{
    startScreen->hide();
    trayIcon->updateUserSettings();
}

void LotusTracker::onUserTokenRefreshError()
{
    startScreen->show();
    trayIcon->updateUserSettings();
}
