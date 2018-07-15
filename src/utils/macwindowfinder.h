#ifndef MACWINDOWFINDER_H
#define MACWINDOWFINDER_H

#include <QObject>

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

class MacWindowFinder : public QObject
{
    Q_OBJECT
public:
    MacWindowFinder();

    static int findWindowId(const QString& name, const QString& title = NULL);
    static bool isWindowFocused(int wId);

};

#endif // MACWINDOWFINDER_H
