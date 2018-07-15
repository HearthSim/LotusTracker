#ifndef WINWINDOWFINDER_H
#define WINWINDOWFINDER_H

#define WIN32_LEAN_AND_MEAN

#include <QObject>
#include <windows.h>

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

class WinWindowFinder : public QObject
{
    Q_OBJECT
public:
  WinWindowFinder();

  static HWND findWindow(const QString& title);
  static bool isWindowFocused(HWND hwnd);

};

#endif // WINWINDOWFINDER_H
