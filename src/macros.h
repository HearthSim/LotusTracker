#ifndef MACROS_H

#define UNUSED(x) (void)(x)

#include "arenatracker.h"
#include <QApplication>

#define LOGI(msg) ((ArenaTracker*) qApp->instance())->logger->logI(msg)
#define LOGD(msg) ((ArenaTracker*) qApp->instance())->logger->logD(msg)
#define LOGW(msg) ((ArenaTracker*) qApp->instance())->logger->logW(msg)

#endif // MACROS_H
