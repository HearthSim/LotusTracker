#include "appsettings.h"
#include "../macros.h"

#define KEY_AUTOSTART "autoStart"

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    LOGD(QString("Settings saved in %1").arg(settings.fileName()))
}

bool AppSettings::isAutoStartEnabled()
{
    return settings.value(KEY_AUTOSTART).toBool();
}

void AppSettings::setAutoStart(bool enabled)
{
    settings.setValue(KEY_AUTOSTART, enabled);
}
