#pragma once

#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include <QApplication>
#include "ui/preferences.h"
#include "trayicon.h"

class ArenaTracker : public QApplication
{
    Q_OBJECT
public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    int run();
    void showPreferences();

private:
    void setupApp();
    TrayIcon *trayIcon;
    Preferences *preferences;

signals:

public slots:
};

#endif // ARENATRACKER_H
