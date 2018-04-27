#include "mtgarena.h"
#include "macros.h"

#if defined Q_OS_MAC
#include "utils/macwindowfinder.h"
#elif defined Q_OS_WIN
#include "utils/winwindowfinder.h"
#include <windows.h>
#endif

#include <QString>

#define MTG_ARENA_NAME "MTGA"
#define MTG_ARENA_TITLE "MTGA"
#define SLOW_FIND_WINDOW_INTERVAL 5000
#define FAST_FIND_WINDOW_INTERVAL 500

MtgArena::MtgArena(QObject *parent) : QObject(parent), isFocused(false), isRunning(false)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MtgArena::findGameWindow);
    timer->start(SLOW_FIND_WINDOW_INTERVAL);
    connect(this, &MtgArena::sgnGameStarted, this, &MtgArena::gameStarted);
    connect(this, &MtgArena::sgnGameStopped, this, &MtgArena::gameStopped);
    connect(this, &MtgArena::sgnGameFocusChanged, this, &MtgArena::gameFocusChanged);
}

void MtgArena::findGameWindow()
{
#if defined Q_OS_MAC
    int wndId = MacWindowFinder::findWindowId(MTG_ARENA_NAME, MTG_ARENA_TITLE);
    bool hasFind = wndId != 0;
    bool hasFocus = MacWindowFinder::isWindowFocused(wndId);
#elif defined Q_OS_WIN
    HWND wnd = WinWindowFinder::findWindow(MTG_ARENA_TITLE);
    bool hasFind = wnd != NULL;
    bool hasFocus = WinWindowFinder::isWindowFocused(wnd);
#endif
    if(!isRunning && hasFind) {
		emit sgnGameStarted();
    }
    if(isFocused != hasFocus){
        emit sgnGameFocusChanged(hasFocus);
    }
    if(isRunning && !hasFind) {
        emit sgnGameStopped();
    }
}

void MtgArena::gameStarted()
{
	LOGI("Game started");
	isRunning = true;
	timer->setInterval(FAST_FIND_WINDOW_INTERVAL);
}

void MtgArena::gameStopped()
{
	LOGI("Game stopped");
	isRunning = false;
	timer->setInterval(SLOW_FIND_WINDOW_INTERVAL);
}

void MtgArena::gameFocusChanged(bool hasFocus)
{
	LOGD(hasFocus ? "Game gains focus" : "Game loses focus");
	isFocused = hasFocus;
}
