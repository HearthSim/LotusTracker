#ifndef MACOSWINDOWFINDER_H
#define MACOSWINDOWFINDER_H

#include <QObject>

class MacOSWindowFinder : public QObject
{
    Q_OBJECT
public:
    MacOSWindowFinder();

    static int findWindowId(const QString& name, const QString& title = NULL);
    static bool isWindowFocused(int wId);

};

#endif // MACOSWINDOWFINDER_H
