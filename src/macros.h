#ifndef MACROS_H

#define UNUSED(x) (void)(x)

#include "arenatracker.h"
#include "utils/appsettings.h"
#include "utils/logger.h"

#include <QApplication>
#include <QDebug>

#define LOG_LEVEL DEBUG //DEBUG, INFO, WARNING
#define LOG_IN_TESTS true
#define RUNNING_TESTS !qApp

#define DEL(__CLASSNAME__) \
if(__CLASSNAME__){ \
    delete __CLASSNAME__; \
    __CLASSNAME__ = NULL; \
} \

#define ARENA_TRACKER ((ArenaTracker*) qApp->instance())

#define APP_SETTINGS ARENA_TRACKER->appSettings

#define LOGD(msg) \
    if (LOG_LEVEL == DEBUG) { \
        if (qApp) { \
            ARENA_TRACKER->logger->logD(__PRETTY_FUNCTION__, __LINE__, msg); \
        } else if (LOG_IN_TESTS) { \
            QString prettyFunction = QString(__PRETTY_FUNCTION__); \
            QString function = prettyFunction.left(prettyFunction.indexOf("::")); \
            qDebug() << QString("%1:%2 - %3").arg(function).arg(__LINE__).arg(msg); \
        } \
    }

#define LOGI(msg) \
    if (LOG_LEVEL == INFO || LOG_LEVEL == DEBUG) { \
        if (qApp) { \
            ARENA_TRACKER->logger->logI(__PRETTY_FUNCTION__, __LINE__, msg); \
        } else if (LOG_IN_TESTS) { \
            QString prettyFunction = QString(__PRETTY_FUNCTION__); \
            QString function = prettyFunction.left(prettyFunction.indexOf("::")); \
            qDebug() << QString("%1:%2 - %3").arg(function).arg(__LINE__).arg(msg); \
        } \
    }

#define LOGW(msg) \
    if (qApp) { \
        ARENA_TRACKER->logger->logW(__PRETTY_FUNCTION__, __LINE__, msg); \
    } else if (LOG_IN_TESTS) { \
        QString prettyFunction = QString(__PRETTY_FUNCTION__); \
        QString function = prettyFunction.left(prettyFunction.indexOf("::")); \
        qDebug() << QString("%1:%2 - %3").arg(function).arg(__LINE__).arg(msg); \
    }

#endif // MACROS_H
