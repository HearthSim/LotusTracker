#include "lotustracker.h"
#include "macros.h"
#include "urls.h"
#include "mtg/mtgalogparser.h"
#include "utils/cocoainitializer.h"

#if defined Q_OS_MAC
#include "utils/macautostart.h"
#include "updater/macsparkleupdater.h"
#elif defined Q_OS_WIN
#include "utils/winautostart.h"
#include "updater/winsparkleupdater.h"
#endif

#include <iostream>
#include <QLocalSocket>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>

#define LOGS_QUEUE_MAX_SIZE 100

LotusTracker::LotusTracker(int& argc, char **argv): QApplication(argc, argv),
    crow_client(nullptr), eventPlayerCourse()
{
#ifdef QT_NO_DEBUG
    std::cout << "Initializing crow";
    crow_client = new nlohmann::crow(CREDENTIALS::GA_SENTRY_DSN().toStdString());
#endif
    setupApp();
    setupUpdater();
    isOnDraftScreen = false;
    logger = new Logger(this);
    appSettings = new AppSettings(this);
    mtgCards = new MtgCards(this);
    mtgDecksArch = new MtgDecksArch(this);
    mtgArena = new MtgArena(this);
    deckOverlayPlayer = new DeckOverlayPlayer();
    deckOverlayOpponent = new DeckOverlayOpponent();
    deckOverlayDraft = new DeckOverlayDraft();
    trayIcon = new TrayIcon(this);
    connect(trayIcon, &TrayIcon::sgnShowDeckOverlay, this, [this]{
        deckOverlayPlayer->show();
        deckOverlayOpponent->show();
    });
    lotusAPI = new LotusTrackerAPI(this);
    startScreen = new StartScreen(nullptr, lotusAPI);
    hideTrackerTimer = new QTimer(this);
    waitPosMatchRankInfoTimer = new QTimer(this);
    mtgaMatch = new MtgaMatch(this, mtgCards);
    gaTracker = new GAnalytics(CREDENTIALS::GA_ID());
    untapped = new Untapped(this);
    connect(mtgArena, &MtgArena::sgnMTGAStarted,
            this, &LotusTracker::onMTGAStarted);
    connect(mtgArena, &MtgArena::sgnMTGAFocusChanged,
            this, &LotusTracker::onGameFocusChanged);
    connect(mtgArena, &MtgArena::sgnMTGAStopped,
            this, &LotusTracker::onGameStopped);
    connect(deckOverlayDraft, &DeckOverlayDraft::sgnRequestPlayerCollection,
            lotusAPI, &LotusTrackerAPI::onRequestPlayerCollection);
    connect(hideTrackerTimer, &QTimer::timeout, this, [this]{
        hideTrackerTimer->stop();
        deckOverlayPlayer->reset();
        deckOverlayPlayer->hide();
        deckOverlayOpponent->reset();
        deckOverlayOpponent->hide();
    });
    connect(waitPosMatchRankInfoTimer, &QTimer::timeout, this, [this]{
        waitPosMatchRankInfoTimer->stop();
        uploadMatch();
    });
    //setupMatch should be called before setupLogParser because sgnMatchInfoResult order
    setupLotusAPIConnections();
    setupLogParserConnections();
    setupMtgaMatchConnections();
    setupPreferencesScreen();
    LOGI("Lotus Tracker started");
#if defined Q_OS_MAC
    MacAutoStart::setEnabled(APP_SETTINGS->isAutoStartEnabled());
#elif defined Q_OS_WIN
    WinAutoStart::setEnabled(APP_SETTINGS->isAutoStartEnabled());
#endif
    // store in a variable, as reading this will clear the first run flag
    bool isFirstRun = APP_SETTINGS->isFirstRun();
    if (isFirstRun) {
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
                untapped->checkForUntappedUploadToken();
            }
        });
        checkConnection->start(3000);
    }

    influx_metric(influxdb_cpp::builder()
        .meas("lt_app_start")
        .tag("autostart", APP_SETTINGS->isAutoStartEnabled() ? "true" : "false")
        .tag("new", isFirstRun ? "true" : "false")
        .field("count", 1)
    );

    checkUntappedTermsOfServices();
}

LotusTracker::~LotusTracker()
{
    DEL(logger)
    DEL(deckOverlayPlayer)
    DEL(deckOverlayOpponent)
    DEL(preferencesScreen)
    DEL(trayIcon)
    DEL(mtgArena)
    DEL(mtgaMatch)
    DEL(lotusAPI)
    DEL(gaTracker)
    if (crow_client) {
        crow_client->~crow();
    }
}

int LotusTracker::run()
{
    try {
        return isAlreadyRunning() ? 1 : exec();
    } catch (const std::exception& ex) {
        trackException(LotusException(ex.what()));
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
  setApplicationName(APP_NAME);
  setApplicationDisplayName(APP_NAME);
  setApplicationVersion(VERSION);
  setOrganizationName("Mtg Lotus Valley");
  setOrganizationDomain(URLs::SITE());
  setWindowIcon(icon);
}

void LotusTracker::setupUpdater()
{
#if defined Q_OS_MAC
    CocoaInitializer cocoaInitializer;
    QString updateUrl = QString("%1/%2").arg(URLs::SITE()).arg("appcast-osx.xml");
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
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnLogFilePathChanged,
            mtgArena, &MtgArena::onLogFilePathChanged);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnRestoreDefaults,
            preferencesScreen->getTabOverlay(), &TabOverlay::onRestoreDefaultsSettings);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnDeckOverlayDraftEnabled,
            this, &LotusTracker::onDeckOverlayDraftEnabledChange);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnDraftOverlaySource,
            deckOverlayDraft, &DeckOverlayDraft::onSourceChanged);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnPlayerOverlayEnabled,
            this, &LotusTracker::onDeckOverlayPlayerEnabledChange);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnRestoreDefaults,
            deckOverlayPlayer, &DeckOverlayPlayer::applyCurrentSettings);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnOpponentOverlayEnabled,
            this, &LotusTracker::onDeckOverlayOpponentEnabledChange);
    connect(preferencesScreen->getTabGeneral(), &TabGeneral::sgnRestoreDefaults,
            deckOverlayOpponent, &DeckOverlayOpponent::applyCurrentSettings);
    // Tab Overlay
    // --- Player Overlay
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerAlpha,
            deckOverlayPlayer, &DeckOverlayBase::changeAlpha);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnUnhideDelay,
            deckOverlayPlayer, &DeckOverlayBase::changeUnhiddenTimeout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerCardLayout,
            deckOverlayPlayer, &DeckOverlayBase::changeCardLayout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardManaCostEnabled,
            deckOverlayPlayer, &DeckOverlayBase::onShowCardManaCostEnabled);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardOnHoverEnabled,
            deckOverlayPlayer, &DeckOverlayBase::onShowCardOnHoverEnabled);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowOnlyRemainingCardsEnabled,
            deckOverlayPlayer, &DeckOverlayPlayer::onShowOnlyRemainingCardsEnabled);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnPlayerTrackerStatistics,
            deckOverlayPlayer, &DeckOverlayPlayer::onStatisticsEnabled);
    // --- Opponent Overlay
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerAlpha,
            deckOverlayOpponent, &DeckOverlayBase::changeAlpha);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnUnhideDelay,
            deckOverlayOpponent, &DeckOverlayBase::changeUnhiddenTimeout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerCardLayout,
            deckOverlayOpponent, &DeckOverlayBase::changeCardLayout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardManaCostEnabled,
            deckOverlayOpponent, &DeckOverlayBase::onShowCardManaCostEnabled);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardOnHoverEnabled,
            deckOverlayOpponent, &DeckOverlayBase::onShowCardOnHoverEnabled);
    // --- Draft Overlay
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerAlpha,
            deckOverlayDraft, &DeckOverlayBase::changeAlpha);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnUnhideDelay,
            deckOverlayDraft, &DeckOverlayBase::changeUnhiddenTimeout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnTrackerCardLayout,
            deckOverlayDraft, &DeckOverlayBase::changeCardLayout);
    connect(preferencesScreen->getTabOverlay(), &TabOverlay::sgnShowCardManaCostEnabled,
            deckOverlayDraft, &DeckOverlayBase::onShowCardManaCostEnabled);
    // Tab Logs
    connect(logger, &Logger::sgnLog,
            preferencesScreen->getTabLogs(), &TabLogs::onNewLog);
    connect(logger, &Logger::sgnLog, this, [this](LogType type, const QString &log){
        if (type == DEBUG) {
            return;
        }
        QString msg = QString("%1 - %2").arg(LOG_TYPE_NAMES[type]).arg(log);
        logsQueue.enqueue(msg);
        if (logsQueue.size() > LOGS_QUEUE_MAX_SIZE) {
            logsQueue.dequeue();
        }
    });
}

void LotusTracker::setupLotusAPIConnections()
{
    connect(lotusAPI, &LotusTrackerAPI::sgnDeckWinRate,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerDeckStatus);
    connect(lotusAPI, &LotusTrackerAPI::sgnEventInfo,
            deckOverlayPlayer, &DeckOverlayPlayer::onReceiveEventInfo);
    connect(lotusAPI, &LotusTrackerAPI::sgnRequestFinishedWithSuccess,
            deckOverlayPlayer, &DeckOverlayPlayer::onLotusAPIRequestFinishedWithSuccess);
    connect(lotusAPI, &LotusTrackerAPI::sgnRequestFinishedWithError,
            deckOverlayPlayer, &DeckOverlayPlayer::onLotusAPIRequestFinishedWithError);
    connect(lotusAPI, &LotusTrackerAPI::sgnEventInfo,
            deckOverlayOpponent, &DeckOverlayOpponent::onReceiveEventInfo);
    connect(lotusAPI, &LotusTrackerAPI::sgnUserLogged,
            this, &LotusTracker::onUserSigned);
    connect(lotusAPI, &LotusTrackerAPI::sgnTokenRefreshed,
            this, &LotusTracker::onUserTokenRefreshed);
    connect(lotusAPI, &LotusTrackerAPI::sgnTokenRefreshError,
            this, &LotusTracker::onUserTokenRefreshError);
    connect(lotusAPI, &LotusTrackerAPI::sgnPlayerCollection,
            deckOverlayDraft, &DeckOverlayDraft::setPlayerCollection);
    connect(lotusAPI, &LotusTrackerAPI::sgnParseDeckPosSideboardJson,
            mtgArena->getLogParser(), &MtgaLogParser::onParseDeckPosSideboardJson);
}

void LotusTracker::setupLogParserConnections()
{
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnMtgaClientVersion,
            mtgArena, &MtgArena::onMtgaClientVersion);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerCollection,
            this, &LotusTracker::onPlayerCollectionUpdated);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDecks,
            this, &LotusTracker::onPlayerDecks);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerInventory,
            lotusAPI, &LotusTrackerAPI::updatePlayerInventory);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckCreated,
            lotusAPI, &LotusTrackerAPI::createPlayerDeck);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckUpdated,
            lotusAPI, &LotusTrackerAPI::updatePlayerDeck);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnActivePlayer,
            mtgaMatch, &MtgaMatch::onActivePlayer);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnDecisionPlayer,
            mtgaMatch, &MtgaMatch::onDecisionPlayer);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnSummarizedMessage,
            mtgaMatch, &MtgaMatch::onSummarizedMessage);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerRankInfo,
            mtgaMatch, &MtgaMatch::onPlayerRankInfo);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerRankUpdated,
            mtgaMatch, &MtgaMatch::onPlayerRankUpdated);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerMythicRatingUpdated,
            mtgaMatch, &MtgaMatch::onPlayerMythicRatingUpdated);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckSubmited,
            this, &LotusTracker::onDeckSubmited);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnPlayerDeckWithSideboardSubmited,
            deckOverlayPlayer, &DeckOverlayPlayer::loadDeckWithSideboard);
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
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnDraftPick,
            this, &LotusTracker::onDraftPick);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnDraftStatus,
            this, &LotusTracker::onDraftStatus);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnDecodeDeckPosSideboardPayload,
            lotusAPI, &LotusTrackerAPI::onDecodeDeckPosSideboardPayload);
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnGoingToHome,
            this, [this](){
        deckOverlayPlayer->reset();
        deckOverlayPlayer->hide();
    });
    connect(mtgArena->getLogParser(), &MtgaLogParser::sgnLeavingDraft,
            this, [this](){
        isOnDraftScreen = false;
        deckOverlayDraft->hide();
    });
}

void LotusTracker::setupMtgaMatchConnections()
{
    // Player
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutOnLibraryCard,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerPutOnLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutOnHandCard,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerPutOnHandCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDrawCard,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerDrawCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDiscardCard,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerDiscardCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerDiscardFromLibraryCard,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerDiscardFromLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnPlayerPutOnBattlefieldCard,
            deckOverlayPlayer, &DeckOverlayPlayer::onPlayerPutOnBattlefieldCard);
    // Opponent
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutOnLibraryCard,
            deckOverlayOpponent, &DeckOverlayOpponent::onOpponentPutOnLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutOnHandCard,
            deckOverlayOpponent, &DeckOverlayOpponent::onOpponentPutOnHandCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPlayCard,
            deckOverlayOpponent, &DeckOverlayOpponent::onOpponentPlayCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentDiscardCard,
            deckOverlayOpponent, &DeckOverlayOpponent::onOpponentDiscardCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentDiscardFromLibraryCard,
            deckOverlayOpponent, &DeckOverlayOpponent::onOpponentDiscardFromLibraryCard);
    connect(mtgaMatch, &MtgaMatch::sgnOpponentPutOnBattlefieldCard,
            deckOverlayOpponent, &DeckOverlayOpponent::onOpponentPutOnBattlefieldCard);
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
    checkUntappedTermsOfServices();
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

void LotusTracker::trackException(LotusException ex)
{
    qDebug() << ex.what();
    gaTracker->sendException(ex.what());
#ifdef QT_NO_DEBUG
    while (logsQueue.size() > 0) {
        QString msg = logsQueue.dequeue();
        crow_client->add_breadcrumb(msg.toStdString());
    }
    crow_client->capture_exception(ex);
#endif
}

void LotusTracker::onMTGAStarted()
{
    gaTracker->startSession();
}

void LotusTracker::onPlayerCollectionUpdated(QMap<int, int> ownedCards)
{
    deckOverlayDraft->setPlayerCollection(ownedCards);
    lotusAPI->updatePlayerCollection(ownedCards);
}

void LotusTracker::onPlayerDecks(QList<Deck> playerDecks)
{
    UNUSED(playerDecks);
    hideTrackerTimer->start(2000);
}

void LotusTracker::onDeckSubmited(QString eventId, Deck deck)
{
    deckOverlayPlayer->loadDeck(deck);
    lotusAPI->updatePlayerDeck(deck);
    lotusAPI->getMatchInfo(eventId, deck.id);
}

void LotusTracker::onEventPlayerCourse(EventPlayerCourse eventPlayerCourse, bool isFinished)
{
    this->eventPlayerCourse = eventPlayerCourse;
    this->untapped->setEventPlayerCourse(eventPlayerCourse);
    if (isFinished && appSettings->isShowDeckAfterDraftEnabled()) {
        deckOverlayPlayer->loadDeck(eventPlayerCourse.currentDeck);
        deckOverlayPlayer->show();
    }
}

void LotusTracker::onMatchStart(QString matchId, QString eventId,
                                QString opponentName, RankInfo opponentInfo)
{
    if (!appSettings->hasAcceptedUntappedToS()) {
        return;
    }
    untapped->checkForUntappedUploadToken();
    isOnDraftScreen = false;
    mtgaMatch->onStartNewMatch(matchId, eventId, opponentName, opponentInfo);
    // Load deck from event in course if not loaded yet (event continues without submitDeck)
    if (eventId == eventPlayerCourse.eventId) {
        Deck deck = eventPlayerCourse.currentDeck;
        deckOverlayPlayer->loadDeck(deck);
        lotusAPI->getMatchInfo(eventId, deck.id);
    }
    deckOverlayOpponent->reset();
    deckOverlayOpponent->setEventId(eventId);
    gaTracker->sendEvent("Match", "starts", eventId);
}

void LotusTracker::onGameStart(GameInfo gameInfo, QList<MatchZone> zones, int seatId)
{
    if (!appSettings->hasAcceptedUntappedToS() || !mtgaMatch->isRunning) {
        return;
    }
    mtgaMatch->onGameStart(gameInfo, zones, seatId);
    LOGD(QString("mtgArena->isFocused: %1").arg(mtgArena->isFocused));
    if (APP_SETTINGS->isDeckOverlayPlayerEnabled() && mtgArena->isFocused) {
        deckOverlayPlayer->show();
    }
    if (APP_SETTINGS->isDeckOverlayOpponentEnabled() && mtgArena->isFocused) {
        deckOverlayOpponent->show();
    }
    if (APP_SETTINGS->isFirstMatch()) {
        showMessage(tr("You can hide/show the overlay with a mouse right click on it."));
    }
    influx_metric(influxdb_cpp::builder()
        .meas("lt_game_start")
        .tag("game_number", QString("%1").arg(gameInfo.number).toStdString())
        .tag("super_format", gameInfo.superFormat.toStdString())
        .tag("match_win_condition", gameInfo.winCondition.toStdString())
        .tag("game_variant", gameInfo.variant.toStdString())
        .tag("game_type", gameInfo.type.toStdString())
        .tag("event_name", mtgaMatch->getMatchDetails().eventId.toStdString())
        .field("match_id", mtgaMatch->getMatchDetails().matchId.toStdString())
        .field("count", 1)
    );

}

void LotusTracker::onGameFocusChanged(bool hasFocus)
{
    if (!appSettings->hasAcceptedUntappedToS()) {
        return;
    }
    if (isOnDraftScreen && APP_SETTINGS->isDeckOverlayDraftEnabled()) {
        if (hasFocus) {
            deckOverlayDraft->show();
        } else if (APP_SETTINGS->isHideOnLoseGameFocusEnabled()) {
            deckOverlayDraft->hide();
        }
    }
    if (mtgaMatch->isRunning && APP_SETTINGS->isDeckOverlayPlayerEnabled()) {
        if (hasFocus) {
            deckOverlayPlayer->show();
        } else if (APP_SETTINGS->isHideOnLoseGameFocusEnabled()) {
            deckOverlayPlayer->hide();
        }
    }
    if (mtgaMatch->isRunning && APP_SETTINGS->isDeckOverlayOpponentEnabled()) {
        if (hasFocus) {
            deckOverlayOpponent->show();
        } else if (APP_SETTINGS->isHideOnLoseGameFocusEnabled()) {
            deckOverlayOpponent->hide();
        }
    }
    gaTracker->sendEvent("Game", "Focus change");
}

void LotusTracker::onGameStopped()
{
    deckOverlayPlayer->hide();
    deckOverlayOpponent->hide();
    gaTracker->endSession();
}

void LotusTracker::onGameCompleted(ResultSpec resultSpec)
{
    if (!appSettings->hasAcceptedUntappedToS() || !mtgaMatch->isRunning) {
        return;
    }
    mtgaMatch->onGameCompleted(deckOverlayPlayer->getDeck(),
                               deckOverlayOpponent->getDeck(), resultSpec);
    hideTrackerTimer->start(5000);
}

void LotusTracker::onMatchEnds(ResultSpec resultSpec)
{
    if (!appSettings->hasAcceptedUntappedToS() || !mtgaMatch->isRunning) {
        return;
    }
    mtgaMatch->onEndCurrentMatch(resultSpec);
    waitPosMatchRankInfoTimer->start(2000);
    gaTracker->sendEvent("Match", "ends");
}

void LotusTracker::uploadMatch()
{
    if (mtgaMatch->getMatchDetails().eventId != "NPE"){
        lotusAPI->uploadMatch(mtgaMatch->getMatchDetails(),
                              deckOverlayPlayer->getDeck(),
                              mtgaMatch->getPlayerRankInfo().first);
    }
    untapped->uploadMatchToUntapped(mtgaMatch->getMatchDetails(),
                                    mtgArena->getLogParser()->getLastMatchLog());
}

void LotusTracker::onEventFinish(QString eventId, QString deckId, QString deckColors,
                                 int maxWins, int wins, int losses)
{
    lotusAPI->uploadEventResult(eventId, deckId, deckColors, maxWins, wins, losses);
    if (APP_SETTINGS->isShowDeckAfterDraftEnabled()) {
        deckOverlayPlayer->reset();
    }
}

void LotusTracker::onDeckOverlayDraftEnabledChange(bool enabled)
{
    if (enabled && isOnDraftScreen) {
        deckOverlayDraft->show();
    } else {
        deckOverlayDraft->hide();
    }
}

void LotusTracker::onDeckOverlayPlayerEnabledChange(bool enabled)
{
    if (enabled && mtgaMatch->isRunning) {
        deckOverlayPlayer->show();
    }
    if (!enabled && mtgaMatch->isRunning) {
        deckOverlayPlayer->hide();
    }
}

void LotusTracker::onDeckOverlayOpponentEnabledChange(bool enabled)
{
    if (enabled && mtgaMatch->isRunning) {
        deckOverlayOpponent->show();
    }
    if (!enabled && mtgaMatch->isRunning) {
        deckOverlayOpponent->hide();
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

void LotusTracker::checkUntappedTermsOfServices()
{
    if (appSettings->hasAcceptedUntappedToS()) {
        return;
    }
    QString msg = "Lotus Tracker is now uploading games for Untapped.gg."
                  "<br/>By continue using Lotus Tracker, you agree with"
                  " Untapped <a href='https://hearthsim.net/legal/terms-of-service.html'>Terms of Service</a>.";
    QMessageBox messageBox("Untapped Terms of Service", msg,QMessageBox::Question,
                           QMessageBox::Ok, QMessageBox::Close, 0);
    messageBox.setTextFormat(Qt::RichText);
    int ret = messageBox.exec();
    if (ret == QMessageBox::Ok) {
        appSettings->acceptUntappedToS();
    } else {
        showMessage("Lotus Tracker can't work without accept terms.");
    }
}

void LotusTracker::onUserSigned(bool fromSignUp)
{
    if (fromSignUp) {
        gaTracker->sendEvent("LotusTracker", "SignUp");
    }
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

void LotusTracker::onDraftPick(int mtgaId, int packNumber, int pickNumber)
{
    QString eventName = deckOverlayDraft->getCurrentDraftName();
    QList<Card *> availablePicks = deckOverlayDraft->getAvailablePicks();
    APP_SETTINGS->setDraftPick(eventName, packNumber, pickNumber, mtgaId, availablePicks);
}

void LotusTracker::onDraftStatus(QString eventName, QString status, int packNumber, int pickNumber,
                                 QList<Card *> availablePicks, QList<Card *> pickedCards)
{
    if (!appSettings->hasAcceptedUntappedToS()) {
        return;
    }
    UNUSED(packNumber);
    UNUSED(pickNumber);
    isOnDraftScreen = true;
    deckOverlayDraft->onDraftStatus(eventName, availablePicks, pickedCards);
    if (status == "Draft.PickNext") {
        deckOverlayDraft->show();
    } else {
        deckOverlayDraft->reset();
        deckOverlayDraft->hide();
    }
}

// Just for Tests
void LotusTracker::setEventInfo(QString eventName, QString eventType)
{
    deckOverlayOpponent->onReceiveEventInfo(eventName, eventType);
}
