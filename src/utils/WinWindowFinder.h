#ifndef WINWINDOWFINDER_H
#define WINWINDOWFINDER_H

#define WIN32_LEAN_AND_MEAN

#include <QObject>
#include <windows.h>

class WinWindowFinder : public QObject
{
    Q_OBJECT
public:
  WinWindowFinder();

  static HWND findWindow(const QString& title);
  static bool isWindowFocused(HWND hwnd);

};

#endif // WINWINDOWFINDER_H
