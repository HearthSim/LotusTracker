#include "appsettings.h"
#include "../macros.h"

#include <QDesktopWidget>

#define KEY_AUTOSTART "autoStart"
#define KEY_AUTOUPDATE "autoUpdate"
#define KEY_FIRST_RUN "isFirstRun"
#define KEY_FIRST_MATCH "isFirstMatch"
#define KEY_HIDE_ON_LOSE_GAME_FOCUS "hideOnLoseGameFocus"
#define KEY_TRACKER_ALPHA "Tracker/alpha"
#define KEY_TRACKER_LAYOUT "Tracker/layout"
#define KEY_TRACKER_UNHIDDEN_DELAY "Tracker/unhiddenDelay"
#define KEY_TRACKER_SHOW_CARD_ON_HOVER "Tracker/showCardOnHover"
#define KEY_TRACKER_SHOW_ONLY_REMAINING_CARDS "Tracker/showOnlyRemainingCards"
#define KEY_TRACKER_SHOW_DEBUG_LOGS "Tracker/showDebugLogs"

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
#define KEY_TRACKER_USER_NAME "Tracker/user/name"

#define DEFAULT_TRACKER_VIEW_X 5
#define DEFAULT_TRACKER_VIEW_Y 110

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    LOGD(QString("Settings saved in %1").arg(settings.fileName()));
}

bool AppSettings::isAutoStartEnabled()
{
    return settings.value(KEY_AUTOSTART, true).toBool();
}

void AppSettings::enableAutoStart(bool enabled)
{
    settings.setValue(KEY_AUTOSTART, enabled);
}

bool AppSettings::isAutoUpdateEnabled()
{
    return settings.value(KEY_AUTOUPDATE, true).toBool();
}

void AppSettings::enableAutoUpdate(bool enabled)
{
    settings.setValue(KEY_AUTOUPDATE, enabled);
}

bool AppSettings::isFirstRun()
{
    bool isFirstRun = settings.value(KEY_FIRST_RUN, true).toBool();
    if (isFirstRun) {
        settings.setValue(KEY_FIRST_RUN, false);
    }
    return isFirstRun;
}

bool AppSettings::isFirstMatch()
{
    bool isFirstMatch = settings.value(KEY_FIRST_MATCH, true).toBool();
    if (isFirstMatch) {
        settings.setValue(KEY_FIRST_MATCH, false);
    }
    return isFirstMatch;
}

bool AppSettings::isHideOnLoseGameFocusEnabled()
{
    return settings.value(KEY_HIDE_ON_LOSE_GAME_FOCUS, true).toBool();
}

void AppSettings::enableHideOnLoseGameFocus(bool enabled)
{
    settings.setValue(KEY_HIDE_ON_LOSE_GAME_FOCUS, enabled);
}

int AppSettings::getDeckTrackerAlpha()
{
    return settings.value(KEY_TRACKER_ALPHA, 7).toInt();
}

void AppSettings::setDeckTrackerAlpha(int alpha)
{
    settings.setValue(KEY_TRACKER_ALPHA, alpha);
}

QString AppSettings::getCardLayout()
{
    return settings.value(KEY_TRACKER_LAYOUT, "mtga").toString();
}

void AppSettings::setCardLayout(QString cardLayout)
{
    settings.setValue(KEY_TRACKER_LAYOUT, cardLayout);
}

int AppSettings::getUnhiddenDelay()
{
    return settings.value(KEY_TRACKER_UNHIDDEN_DELAY, 5).toInt();
}

void AppSettings::setUnhiddenDelay(int unhiddenDelay)
{
    settings.setValue(KEY_TRACKER_UNHIDDEN_DELAY, unhiddenDelay);
}

bool AppSettings::isShowCardOnHoverEnabled()
{
    return settings.value(KEY_TRACKER_SHOW_CARD_ON_HOVER, true).toBool();
}

void AppSettings::enableShowCardOnHover(bool enabled)
{
    settings.setValue(KEY_TRACKER_SHOW_CARD_ON_HOVER, enabled);
}

bool AppSettings::isShowOnlyRemainingCardsEnabled()
{
    return settings.value(KEY_TRACKER_SHOW_ONLY_REMAINING_CARDS, true).toBool();
}

void AppSettings::enableShowOnlyRemainingCards(bool enabled)
{
    settings.setValue(KEY_TRACKER_SHOW_ONLY_REMAINING_CARDS, enabled);
}

bool AppSettings::isShowDebugLogsEnabled()
{
    return settings.value(KEY_TRACKER_SHOW_DEBUG_LOGS, false).toBool();
}

void AppSettings::enableShowDebugLogs(bool enabled)
{
    settings.setValue(KEY_TRACKER_SHOW_DEBUG_LOGS, enabled);
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
    return settings.value(KEY_TRACKER_PLAYER_STATISTICS, false).toBool();
}

void AppSettings::enableDeckTrackerPlayerStatistics(bool enabled)
{
    settings.setValue(KEY_TRACKER_PLAYER_STATISTICS, enabled);
}

QPoint AppSettings::getDeckTrackerPlayerPos(int uiWidth)
{
    UNUSED(uiWidth);
    int x = settings.value(KEY_TRACKER_PLAYER_X, DEFAULT_TRACKER_VIEW_X).toInt();
    int y = settings.value(KEY_TRACKER_PLAYER_Y, DEFAULT_TRACKER_VIEW_Y).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckTrackerPlayerPos(QPoint pos)
{
    settings.setValue(KEY_TRACKER_PLAYER_X, pos.x());
    settings.setValue(KEY_TRACKER_PLAYER_Y, pos.y());
}

int AppSettings::getDeckTrackerPlayerScale()
{
    return settings.value(KEY_TRACKER_PLAYER_SCALE, 1).toInt();
}

void AppSettings::setDeckTrackerPlayerScale(int scale)
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
    int defaultX = screen.width() - uiWidth - DEFAULT_TRACKER_VIEW_X;
    int x = settings.value(KEY_TRACKER_OPPONENT_X, defaultX).toInt();
    int y = settings.value(KEY_TRACKER_OPPONENT_Y, DEFAULT_TRACKER_VIEW_Y).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckTrackerOpponentPos(QPoint pos)
{
    settings.setValue(KEY_TRACKER_OPPONENT_X, pos.x());
    settings.setValue(KEY_TRACKER_OPPONENT_Y, pos.y());
}

int AppSettings::getDeckTrackerOpponentScale()
{
    return settings.value(KEY_TRACKER_OPPONENT_SCALE, 1).toInt();
}

void AppSettings::setDeckTrackerOpponentScale(int scale)
{
    settings.setValue(KEY_TRACKER_OPPONENT_SCALE, scale);
}

void AppSettings::setUserSettings(UserSettings userSettings, QString userName)
{
    settings.setValue(KEY_TRACKER_USER_ID, userSettings.userId);
    settings.setValue(KEY_TRACKER_USER_TOKEN, userSettings.userToken);
    settings.setValue(KEY_TRACKER_USER_REFRESH_TOKEN, userSettings.refreshToken);
    settings.setValue(KEY_TRACKER_USER_EXPIRES_EPOCH, userSettings.expiresTokenEpoch);
    if (!userName.isEmpty()) {
        settings.setValue(KEY_TRACKER_USER_NAME, userName);
    }
}

UserSettings AppSettings::getUserSettings()
{
    UserSettings userSettings = UserSettings(settings.value(KEY_TRACKER_USER_ID, "").toString(),
                        settings.value(KEY_TRACKER_USER_TOKEN, "").toString(),
                        settings.value(KEY_TRACKER_USER_REFRESH_TOKEN, "").toString(),
                        settings.value(KEY_TRACKER_USER_EXPIRES_EPOCH, 0).toLongLong());
    userSettings.setUserName(settings.value(KEY_TRACKER_USER_NAME, "").toString());
    return userSettings;
}

void AppSettings::clearUserSettings()
{
    setUserSettings(UserSettings(), "");
}

void AppSettings::restoreDefaults()
{
    settings.remove(KEY_AUTOSTART);
    settings.remove(KEY_TRACKER_ALPHA);
    settings.remove(KEY_TRACKER_LAYOUT);
    settings.remove(KEY_TRACKER_PLAYER_ENABLED);
    settings.remove(KEY_TRACKER_SHOW_ONLY_REMAINING_CARDS);
    settings.remove(KEY_TRACKER_PLAYER_STATISTICS);
    settings.remove(KEY_TRACKER_PLAYER_X);
    settings.remove(KEY_TRACKER_PLAYER_Y);
    settings.remove(KEY_TRACKER_PLAYER_SCALE);
    settings.remove(KEY_TRACKER_OPPONENT_ENABLED);
    settings.remove(KEY_TRACKER_OPPONENT_X);
    settings.remove(KEY_TRACKER_OPPONENT_Y);
    settings.remove(KEY_TRACKER_OPPONENT_SCALE);
}
