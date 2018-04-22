#pragma once

#ifndef ARENATRACKER_H
#define ARENATRACKER_H

#include <QApplication>
#include "settings.h"

class ArenaTracker : public QApplication
{
    Q_OBJECT
public:
    ArenaTracker(int& argc, char **argv);
    ~ArenaTracker();
    int run();

private:
    Settings *settings;
    void setup();

signals:

public slots:
};

#endif // ARENATRACKER_H
