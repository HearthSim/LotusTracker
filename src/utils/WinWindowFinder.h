#ifndef WINWINDOWFINDER_H
#define WINWINDOWFINDER_H

#include <QObject>
#include <Windows.h>

class WinWindowFinder : public QObject
{
    Q_OBJECT
public:
  WinWindowFinder();

  static HWND FindWindow(const QString& name, const QString& title = NULL);
  static bool isWindowFocused(HWND hwnd);

};

#endif // WINWINDOWFINDER_H
