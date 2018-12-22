#ifndef MACROS_H

#define UNUSED(x) (void)(x)

#include "lotustracker.h"
#include "utils/appsettings.h"
#include "utils/logger.h"

#include <QApplication>
#include <QDebug>

#define APP_NAME "Lotus Tracker"
#define LOG_LEVEL DEBUG //DEBUG, INFO, WARNING
#define LOG_IN_TESTS true
#define LOG_DECK_ARCH_CALC false
#define LOG_REQUEST_ENABLED false
#define RUNNING_TESTS !qApp

#define DEL(__CLASSNAME__) \
if(__CLASSNAME__){ \
    delete __CLASSNAME__; \
    __CLASSNAME__ = NULL; \
} \

#define LOTUS_TRACKER dynamic_cast<LotusTracker*>(qApp->instance())

#define APP_SETTINGS LOTUS_TRACKER->appSettings

#define LOGD(msg) \
    if (LOG_LEVEL == DEBUG) { \
        if (qApp) { \
            LOTUS_TRACKER->logger->logD(__PRETTY_FUNCTION__, __LINE__, msg); \
        } else if (LOG_IN_TESTS) { \
            QString prettyFunction = QString(__PRETTY_FUNCTION__); \
            QString function = prettyFunction.left(prettyFunction.indexOf("::")); \
            qDebug() << QString("%1:%2 - %3").arg(function).arg(__LINE__).arg(msg); \
        } \
    }

#define LOGI(msg) \
    if (LOG_LEVEL == INFO || LOG_LEVEL == DEBUG) { \
        if (qApp) { \
            LOTUS_TRACKER->logger->logI(__PRETTY_FUNCTION__, __LINE__, msg); \
        } else if (LOG_IN_TESTS) { \
            QString prettyFunction = QString(__PRETTY_FUNCTION__); \
            QString function = prettyFunction.left(prettyFunction.indexOf("::")); \
            qDebug() << QString("%1:%2 - %3").arg(function).arg(__LINE__).arg(msg); \
        } \
    }

#define LOGW(msg) \
    if (qApp) { \
        LOTUS_TRACKER->logger->logW(__PRETTY_FUNCTION__, __LINE__, msg); \
    } else if (LOG_IN_TESTS) { \
        QString prettyFunction = QString(__PRETTY_FUNCTION__); \
        QString function = prettyFunction.left(prettyFunction.indexOf("::")); \
        qDebug() << QString("%1:%2 - %3").arg(function).arg(__LINE__).arg(msg); \
    }

#endif // MACROS_H
