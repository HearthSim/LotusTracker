#ifndef MACROS_H

#define UNUSED(x) (void)(x)

#include "arenatracker.h"
#include "logger.h"

#include <QApplication>

#define LOGI(msg) ((ArenaTracker*) qApp->instance())->logger->logI(__PRETTY_FUNCTION__, __LINE__, msg)
#define LOGD(msg) ((ArenaTracker*) qApp->instance())->logger->logD(__PRETTY_FUNCTION__, __LINE__, msg)
#define LOGW(msg) ((ArenaTracker*) qApp->instance())->logger->logW(__PRETTY_FUNCTION__, __LINE__, msg)

#define DELETE(__CLASSNAME__) \
if(__CLASSNAME__){ \
    delete __CLASSNAME__; \
    __CLASSNAME__ = NULL; \
} \

#endif // MACROS_H
