#ifndef MACWINDOWFINDER_H
#define MACWINDOWFINDER_H

#include <QObject>

class MacWindowFinder : public QObject
{
    Q_OBJECT
public:
    MacWindowFinder();

    static int findWindowId(const QString& name, const QString& title = NULL);
    static bool isWindowFocused(int wId);

};

#endif // MACWINDOWFINDER_H
