#include "appsettings.h"
#include "../macros.h"

#include <QDesktopWidget>

#define KEY_AUTOSTART "autoStart"
#define KEY_TRACKER_LAYOUT "Tracker/layout"

#define KEY_TRACKER_PLAYER_ENABLED "Tracker/playerPrefs/enabled"
#define KEY_TRACKER_PLAYER_STATISTICS "Tracker/playerPrefs/statistics"
#define KEY_TRACKER_PLAYER_X "Tracker/playerPrefs/x"
#define KEY_TRACKER_PLAYER_Y "Tracker/playerPrefs/y"
#define KEY_TRACKER_PLAYER_SCALE "Tracker/playerPrefs/scale"

#define KEY_TRACKER_OPPONENT_ENABLED "Tracker/opponentPrefs/enabled"
#define KEY_TRACKER_OPPONENT_X "Tracker/opponentPrefs/x"
#define KEY_TRACKER_OPPONENT_Y "Tracker/opponentPrefs/y"
#define KEY_TRACKER_OPPONENT_SCALE "Tracker/opponentPrefs/scale"

#define KEY_TRACKER_USER_ID "Tracker/user/id"
#define KEY_TRACKER_USER_TOKEN "Tracker/user/token"
#define KEY_TRACKER_USER_REFRESH_TOKEN "Tracker/user/refreshToken"
#define KEY_TRACKER_USER_EXPIRES_EPOCH "Tracker/user/tokenExpiresEpoch"

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    LOGD(QString("Settings saved in %1").arg(settings.fileName()));
}

bool AppSettings::isAutoStartEnabled()
{
    return settings.value(KEY_AUTOSTART, false).toBool();
}

void AppSettings::enableAutoStart(bool enabled)
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

// Deck tracker player

bool AppSettings::isDeckTrackerPlayerEnabled()
{
    return settings.value(KEY_TRACKER_PLAYER_ENABLED, true).toBool();
}

void AppSettings::enableDeckTrackerPlayer(bool enabled)
{
    settings.setValue(KEY_TRACKER_PLAYER_ENABLED, enabled);
}

bool AppSettings::isDeckTrackerPlayerStatisticsEnabled()
{
    return settings.value(KEY_TRACKER_PLAYER_STATISTICS, true).toBool();
}

void AppSettings::enableDeckTrackerPlayerStatistics(bool enabled)
{
    settings.setValue(KEY_TRACKER_PLAYER_STATISTICS, enabled);
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

// Deck tracker opponent

bool AppSettings::isDeckTrackerOpponentEnabled()
{
    return settings.value(KEY_TRACKER_OPPONENT_ENABLED, true).toBool();
}

void AppSettings::enableDeckTrackerOpponent(bool enabled)
{
    settings.setValue(KEY_TRACKER_OPPONENT_ENABLED, enabled);
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

void AppSettings::setUserSettings(QString userId, QString userToken,
                                  QString refreshToken, qlonglong expiresTimeEpoch)
{
    settings.setValue(KEY_TRACKER_USER_ID, userId);
    settings.setValue(KEY_TRACKER_USER_TOKEN, userToken);
    settings.setValue(KEY_TRACKER_USER_REFRESH_TOKEN, refreshToken);
    settings.setValue(KEY_TRACKER_USER_EXPIRES_EPOCH, expiresTimeEpoch);
}

UserSettings AppSettings::getUserSettings()
{
    return UserSettings(settings.value(KEY_TRACKER_USER_ID, "").toString(),
                        settings.value(KEY_TRACKER_USER_TOKEN, "").toString(),
                        settings.value(KEY_TRACKER_USER_REFRESH_TOKEN, "").toString(),
                        settings.value(KEY_TRACKER_USER_EXPIRES_EPOCH, 0).toLongLong());
}
