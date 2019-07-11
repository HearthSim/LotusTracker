#include "mtgarena.h"
#include "mtgcards.h"
#include "../macros.h"

#if defined Q_OS_MAC
#include "../utils/macwindowfinder.h"
#elif defined Q_OS_WIN
#include "../utils/winwindowfinder.h"
#include <windows.h>
#endif

#define MTG_ARENA_NAME "MTGA"
#define MTG_ARENA_TITLE "MTGA"
#define SLOW_FIND_WINDOW_INTERVAL 5000
#define FAST_FIND_WINDOW_INTERVAL 1000

MtgArena::MtgArena(QObject *parent): QObject(parent),
     isFocused(false), isRunning(false)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MtgArena::findGameWindow);
    timer->start(SLOW_FIND_WINDOW_INTERVAL);
    connect(this, &MtgArena::sgnMTGAStarted,
            this, &MtgArena::onMtgaStarted);
    connect(this, &MtgArena::sgnMTGAStopped,
            this, &MtgArena::onMtgaStopped);
    connect(this, &MtgArena::sgnMTGAFocusChanged,
            this, &MtgArena::onMtgaFocusChanged);
    logParser = new MtgaLogParser(this, LOTUS_TRACKER->mtgCards);
    logWatcher = new MtgaLogWatcher(this);
    connect(logWatcher, &MtgaLogWatcher::sgnNewLogContent, this, &MtgArena::onNewLogContent);
}

MtgArena::~MtgArena()
{
    DEL(timer);
    DEL(logParser);
    DEL(logWatcher);
}

QString MtgArena::getClientVersion()
{
    return mtgaVersion;
}

MtgaLogParser* MtgArena::getLogParser()
{
    return logParser;
}

void MtgArena::onLogFilePathChanged(QString logPath)
{
    logWatcher->setLogPath(logPath);
    logWatcher->stopWatching();
}

void MtgArena::onMtgaClientVersion(QString version)
{
    if (version != mtgaVersion) {
        LOGI(mtgaVersion);
    }
    mtgaVersion = version;
}

void MtgArena::findGameWindow()
{
#if defined Q_OS_MAC
    int wndId = MacWindowFinder::findWindowId(MTG_ARENA_TITLE);
    bool hasFind = wndId != 0;
    bool hasFocus = MacWindowFinder::isWindowFocused(wndId);
#elif defined Q_OS_WIN
    HWND wnd = WinWindowFinder::findWindow(MTG_ARENA_NAME, MTG_ARENA_TITLE);
    bool hasFind = wnd != NULL;
    bool hasFocus = WinWindowFinder::isWindowFocused(wnd);
#endif
    if (!isRunning && hasFind) {
        emit sgnMTGAStarted();
    }
    if (isFocused != hasFocus){
        onCurrentFocusChanged(hasFocus);
    }
    if (isRunning && !hasFind) {
        emit sgnMTGAStopped();
    }
}

void MtgArena::onCurrentFocusChanged(bool hasFocus)
{
    QString overlayTitle = QString("%1 - %2").arg(DeckOverlayBase::TITLE()).arg(APP_NAME);
    QString preferencesTitle = QString("%1 - %2").arg(PreferencesScreen::TITLE()).arg(APP_NAME);
#if defined Q_OS_MAC
    int wndId = MacWindowFinder::findWindowId(overlayTitle);
    bool hasTrackerOverlayFocus = MacWindowFinder::isWindowFocused(wndId);
    wndId = MacWindowFinder::findWindowId(preferencesTitle);
    bool hasLotusTrackerFocus = MacWindowFinder::isWindowFocused(wndId);
#elif defined Q_OS_WIN
    HWND wnd = WinWindowFinder::findWindow(NULL, overlayTitle);
    bool hasTrackerOverlayFocus = WinWindowFinder::isWindowFocused(wnd);
    wnd = WinWindowFinder::findWindow(NULL, preferencesTitle);
    bool hasLotusTrackerFocus = WinWindowFinder::isWindowFocused(wnd);
#endif
    if (!hasFocus && (hasTrackerOverlayFocus || hasLotusTrackerFocus)) {
        return;
    }
    emit sgnMTGAFocusChanged(hasFocus);
}

void MtgArena::onMtgaStarted()
{
	LOGI("Game started");
	isRunning = true;
	timer->setInterval(FAST_FIND_WINDOW_INTERVAL);
    LOTUS_TRACKER->mtgCards->updateMtgaSetsFromAPI();
    LOTUS_TRACKER->mtgDecksArch->updateDecksArchitectureFromAPI();
}

void MtgArena::onMtgaStopped()
{
	LOGI("Game stopped");
	isRunning = false;
	timer->setInterval(SLOW_FIND_WINDOW_INTERVAL);
}

void MtgArena::onMtgaFocusChanged(bool hasFocus)
{
	LOGD(hasFocus ? "Game gains focus" : "Game loses focus");
	isFocused = hasFocus;
}

void MtgArena::onNewLogContent(QString logNewContent)
{
    logParser->parse(logNewContent);
}
