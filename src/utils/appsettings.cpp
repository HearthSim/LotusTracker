#include "appsettings.h"
#include "../macros.h"

#define KEY_AUTOSTART "autoStart"
#define KEY_TRACKER_LAYOUT "Tracker/layout"

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    LOGD(QString("Settings saved in %1").arg(settings.fileName()));
    if (!settings.contains(KEY_TRACKER_LAYOUT)) {
        setCardLayout("mtga");
    }
}

bool AppSettings::isAutoStartEnabled()
{
    return settings.value(KEY_AUTOSTART).toBool();
}

void AppSettings::setAutoStart(bool enabled)
{
    settings.setValue(KEY_AUTOSTART, enabled);
}

QString AppSettings::getCardLayout()
{
    return settings.value(KEY_TRACKER_LAYOUT).toString();
}

void AppSettings::setCardLayout(QString cardLayout)
{
    settings.setValue(KEY_TRACKER_LAYOUT, cardLayout);
}
