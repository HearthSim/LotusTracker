#include "appsettings.h"
#include "../macros.h"

#include <QDesktopWidget>

#define KEY_AUTOSTART "autoStart"
#define KEY_TRACKER_LAYOUT "Tracker/layout"
#define KEY_TRACKER_PLAYER_X "Tracker/playerPrefs/x"
#define KEY_TRACKER_PLAYER_Y "Tracker/playerPrefs/y"
#define KEY_TRACKER_PLAYER_SCALE "Tracker/playerPrefs/scale"
#define KEY_TRACKER_OPPONENT_X "Tracker/opponentPrefs/x"
#define KEY_TRACKER_OPPONENT_Y "Tracker/opponentPrefs/y"
#define KEY_TRACKER_OPPONENT_SCALE "Tracker/opponentPrefs/scale"

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    LOGD(QString("Settings saved in %1").arg(settings.fileName()));
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
    return settings.value(KEY_TRACKER_LAYOUT, "mtga").toString();
}

void AppSettings::setCardLayout(QString cardLayout)
{
    settings.setValue(KEY_TRACKER_LAYOUT, cardLayout);
}

QPoint AppSettings::getDeckTrackerPlayerPos()
{
    int x = settings.value(KEY_TRACKER_PLAYER_X, 10).toInt();
    int y = settings.value(KEY_TRACKER_PLAYER_Y, 10).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckTrackerPlayerPos(QPoint pos)
{
    settings.setValue(KEY_TRACKER_PLAYER_X, pos.x());
    settings.setValue(KEY_TRACKER_PLAYER_Y, pos.y());
}

qreal AppSettings::getDeckTrackerPlayerScale()
{
    return settings.value(KEY_TRACKER_PLAYER_SCALE, 1).toReal();
}

void AppSettings::setDeckTrackerPlayerScale(qreal scale)
{
    settings.setValue(KEY_TRACKER_PLAYER_SCALE, scale);
}

QPoint AppSettings::getDeckTrackerOpponentPos(int uiWidth)
{
    QRect screen = QApplication::desktop()->screenGeometry();
    int defaultX = screen.width() - uiWidth - 10;
    int x = settings.value(KEY_TRACKER_OPPONENT_X, defaultX).toInt();
    int y = settings.value(KEY_TRACKER_OPPONENT_Y, 10).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckTrackerOpponentPos(QPoint pos)
{
    settings.setValue(KEY_TRACKER_OPPONENT_X, pos.x());
    settings.setValue(KEY_TRACKER_OPPONENT_Y, pos.y());
}

qreal AppSettings::getDeckTrackerOpponentScale()
{
    return settings.value(KEY_TRACKER_OPPONENT_SCALE, 1).toReal();
}

void AppSettings::setDeckTrackerOpponentScale(qreal scale)
{
    settings.setValue(KEY_TRACKER_OPPONENT_SCALE, scale);
}
