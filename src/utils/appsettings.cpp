#include "appsettings.h"
#include "../macros.h"

#include <QDesktopWidget>

#define KEY_AUTOSTART "autoStart"
#define KEY_AUTOUPDATE "autoUpdate"
#define KEY_FIRST_RUN "isFirstRun"
#define KEY_FIRST_MATCH "isFirstMatch"
#define KEY_HIDE_ON_LOSE_GAME_FOCUS "hideOnLoseGameFocus"
#define KEY_OVERLAY_ALPHA "Tracker/alpha"
#define KEY_OVERLAY_LAYOUT "Tracker/layout"
#define KEY_OVERLAY_UNHIDDEN_DELAY "Tracker/unhiddenDelay"
#define KEY_OVERLAY_SHOW_CARD_ON_HOVER "Tracker/showCardOnHover"
#define KEY_OVERLAY_SHOW_CARD_MANA_COST "Tracker/showCardManaCost"
#define KEY_OVERLAY_SHOW_ONLY_REMAINING_CARDS "Tracker/showOnlyRemainingCards"
#define KEY_OVERLAY_SHOW_DEBUG_LOGS "Tracker/showDebugLogs"

#define KEY_OVERLAY_PLAYER_ENABLED "Tracker/playerPrefs/enabled"
#define KEY_OVERLAY_PLAYER_STATISTICS "Tracker/playerPrefs/statistics"
#define KEY_OVERLAY_PLAYER_X "Tracker/playerPrefs/x"
#define KEY_OVERLAY_PLAYER_Y "Tracker/playerPrefs/y"
#define KEY_OVERLAY_PLAYER_SCALE "Tracker/playerPrefs/scale"

#define KEY_OVERLAY_OPPONENT_ENABLED "Tracker/opponentPrefs/enabled"
#define KEY_OVERLAY_OPPONENT_X "Tracker/opponentPrefs/x"
#define KEY_OVERLAY_OPPONENT_Y "Tracker/opponentPrefs/y"
#define KEY_OVERLAY_OPPONENT_SCALE "Tracker/opponentPrefs/scale"

#define KEY_OVERLAY_DRAFT_ENABLED "Tracker/draftPrefs/enabled"
#define KEY_OVERLAY_DRAFT_SOURCE "Tracker/draftPrefs/source"
#define KEY_OVERLAY_DRAFT_X "Tracker/draftPrefs/x"
#define KEY_OVERLAY_DRAFT_Y "Tracker/draftPrefs/y"
#define KEY_OVERLAY_DRAFT_SCALE "Tracker/draftPrefs/scale"

#define KEY_OVERLAY_USER_ID "Tracker/user/id"
#define KEY_OVERLAY_USER_EMAIL "Tracker/user/email"
#define KEY_OVERLAY_USER_TOKEN "Tracker/user/token"
#define KEY_OVERLAY_USER_REFRESH_TOKEN "Tracker/user/refreshToken"
#define KEY_OVERLAY_USER_EXPIRES_EPOCH "Tracker/user/tokenExpiresEpoch"
#define KEY_OVERLAY_USER_NAME "Tracker/user/name"

#define DEFAULT_OVERLAY_VIEW_X 5
#define DEFAULT_OVERLAY_VIEW_Y 60

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
    return settings.value(KEY_OVERLAY_ALPHA, 7).toInt();
}

void AppSettings::setDeckTrackerAlpha(int alpha)
{
    settings.setValue(KEY_OVERLAY_ALPHA, alpha);
}

QString AppSettings::getCardLayout()
{
    return settings.value(KEY_OVERLAY_LAYOUT, "mtga").toString();
}

void AppSettings::setCardLayout(QString cardLayout)
{
    settings.setValue(KEY_OVERLAY_LAYOUT, cardLayout);
}

int AppSettings::getUnhiddenDelay()
{
    return settings.value(KEY_OVERLAY_UNHIDDEN_DELAY, 0).toInt();
}

void AppSettings::setUnhiddenDelay(int unhiddenDelay)
{
    settings.setValue(KEY_OVERLAY_UNHIDDEN_DELAY, unhiddenDelay);
}

bool AppSettings::isShowCardManaCostEnabled()
{
    return settings.value(KEY_OVERLAY_SHOW_CARD_MANA_COST, true).toBool();
}

void AppSettings::enableShowCardManaCost(bool enabled)
{
    settings.setValue(KEY_OVERLAY_SHOW_CARD_MANA_COST, enabled);
}

bool AppSettings::isShowCardOnHoverEnabled()
{
    return settings.value(KEY_OVERLAY_SHOW_CARD_ON_HOVER, true).toBool();
}

void AppSettings::enableShowCardOnHover(bool enabled)
{
    settings.setValue(KEY_OVERLAY_SHOW_CARD_ON_HOVER, enabled);
}

bool AppSettings::isShowOnlyRemainingCardsEnabled()
{
    return settings.value(KEY_OVERLAY_SHOW_ONLY_REMAINING_CARDS, true).toBool();
}

void AppSettings::enableShowOnlyRemainingCards(bool enabled)
{
    settings.setValue(KEY_OVERLAY_SHOW_ONLY_REMAINING_CARDS, enabled);
}

bool AppSettings::isShowDebugLogsEnabled()
{
    return settings.value(KEY_OVERLAY_SHOW_DEBUG_LOGS, false).toBool();
}

void AppSettings::enableShowDebugLogs(bool enabled)
{
    settings.setValue(KEY_OVERLAY_SHOW_DEBUG_LOGS, enabled);
}

// Deck overlay player

bool AppSettings::isDeckOverlayPlayerEnabled()
{
    return settings.value(KEY_OVERLAY_PLAYER_ENABLED, true).toBool();
}

void AppSettings::enableDeckOverlayPlayer(bool enabled)
{
    settings.setValue(KEY_OVERLAY_PLAYER_ENABLED, enabled);
}

bool AppSettings::isDeckOverlayPlayerStatisticsEnabled()
{
    return settings.value(KEY_OVERLAY_PLAYER_STATISTICS, false).toBool();
}

void AppSettings::enableDeckOverlayPlayerStatistics(bool enabled)
{
    settings.setValue(KEY_OVERLAY_PLAYER_STATISTICS, enabled);
}

QPoint AppSettings::getDeckOverlayPlayerPos(int uiWidth)
{
    UNUSED(uiWidth);
    int x = settings.value(KEY_OVERLAY_PLAYER_X, DEFAULT_OVERLAY_VIEW_X).toInt();
    int y = settings.value(KEY_OVERLAY_PLAYER_Y, DEFAULT_OVERLAY_VIEW_Y).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckOverlayPlayerPos(QPoint pos)
{
    settings.setValue(KEY_OVERLAY_PLAYER_X, pos.x());
    settings.setValue(KEY_OVERLAY_PLAYER_Y, pos.y());
}

int AppSettings::getDeckOverlayPlayerScale()
{
    return settings.value(KEY_OVERLAY_PLAYER_SCALE, 1).toInt();
}

void AppSettings::setDeckOverlayPlayerScale(int scale)
{
    settings.setValue(KEY_OVERLAY_PLAYER_SCALE, scale);
}

// Deck overlay opponent

bool AppSettings::isDeckOverlayOpponentEnabled()
{
    return settings.value(KEY_OVERLAY_OPPONENT_ENABLED, true).toBool();
}

void AppSettings::enableDeckOverlayOpponent(bool enabled)
{
    settings.setValue(KEY_OVERLAY_OPPONENT_ENABLED, enabled);
}

QPoint AppSettings::getDeckOverlayOpponentPos(int uiWidth, int cardHoverWidth)
{
    QRect screen = QApplication::desktop()->screenGeometry();
    int defaultX = screen.width() - uiWidth - cardHoverWidth - DEFAULT_OVERLAY_VIEW_X - 25;
    int x = settings.value(KEY_OVERLAY_OPPONENT_X, defaultX).toInt();
    int y = settings.value(KEY_OVERLAY_OPPONENT_Y, DEFAULT_OVERLAY_VIEW_Y).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckOverlayOpponentPos(QPoint pos)
{
    settings.setValue(KEY_OVERLAY_OPPONENT_X, pos.x());
    settings.setValue(KEY_OVERLAY_OPPONENT_Y, pos.y());
}

int AppSettings::getDeckOverlayOpponentScale()
{
    return settings.value(KEY_OVERLAY_OPPONENT_SCALE, 1).toInt();
}

void AppSettings::setDeckOverlayOpponentScale(int scale)
{
    settings.setValue(KEY_OVERLAY_OPPONENT_SCALE, scale);
}

// Draft overlay

bool AppSettings::isDeckOverlayDraftEnabled()
{
    return settings.value(KEY_OVERLAY_DRAFT_ENABLED, true).toBool();
}

void AppSettings::enableDeckOverlayDraft(bool enabled)
{
    settings.setValue(KEY_OVERLAY_DRAFT_ENABLED, enabled);
}

QString AppSettings::getDeckOverlayDraftSource()
{
    return settings.value(KEY_OVERLAY_DRAFT_SOURCE, "lsv").toString();
}

void AppSettings::setDeckOverlayDraftSource(QString source)
{
    settings.setValue(KEY_OVERLAY_DRAFT_SOURCE, source);
}

QPoint AppSettings::getDeckOverlayDraftPos(int uiWidth)
{
    UNUSED(uiWidth);
    int x = settings.value(KEY_OVERLAY_DRAFT_X, DEFAULT_OVERLAY_VIEW_X).toInt();
    int y = settings.value(KEY_OVERLAY_DRAFT_Y, DEFAULT_OVERLAY_VIEW_Y).toInt();
    return QPoint(x, y);
}

void AppSettings::setDeckOverlayDraftPos(QPoint pos)
{
    settings.setValue(KEY_OVERLAY_DRAFT_X, pos.x());
    settings.setValue(KEY_OVERLAY_DRAFT_Y, pos.y());
}

int AppSettings::getDeckOverlayDraftScale()
{
    return settings.value(KEY_OVERLAY_DRAFT_SCALE, 1).toInt();
}

void AppSettings::setDeckOverlayDraftScale(int scale)
{
    settings.setValue(KEY_OVERLAY_DRAFT_SCALE, scale);
}

// User settings

void AppSettings::setUserSettings(UserSettings userSettings, QString userName)
{
    settings.setValue(KEY_OVERLAY_USER_ID, userSettings.userId);
    settings.setValue(KEY_OVERLAY_USER_EMAIL, userSettings.userEmail);
    settings.setValue(KEY_OVERLAY_USER_TOKEN, userSettings.userToken);
    settings.setValue(KEY_OVERLAY_USER_REFRESH_TOKEN, userSettings.refreshToken);
    settings.setValue(KEY_OVERLAY_USER_EXPIRES_EPOCH, userSettings.expiresTokenEpoch);
    if (!userName.isEmpty()) {
        settings.setValue(KEY_OVERLAY_USER_NAME, userName);
    }
}

UserSettings AppSettings::getUserSettings()
{
    UserSettings userSettings = UserSettings(settings.value(KEY_OVERLAY_USER_ID, "").toString(),
                        settings.value(KEY_OVERLAY_USER_EMAIL, "").toString(),
                        settings.value(KEY_OVERLAY_USER_TOKEN, "").toString(),
                        settings.value(KEY_OVERLAY_USER_REFRESH_TOKEN, "").toString(),
                        settings.value(KEY_OVERLAY_USER_EXPIRES_EPOCH, 0).toLongLong());
    userSettings.setUserName(settings.value(KEY_OVERLAY_USER_NAME, "").toString());
    return userSettings;
}

void AppSettings::clearUserSettings()
{
    setUserSettings(UserSettings(), "");
}

void AppSettings::restoreDefaults()
{
    settings.remove(KEY_AUTOSTART);
    settings.remove(KEY_OVERLAY_ALPHA);
    settings.remove(KEY_OVERLAY_LAYOUT);
    settings.remove(KEY_OVERLAY_PLAYER_ENABLED);
    settings.remove(KEY_OVERLAY_SHOW_ONLY_REMAINING_CARDS);
    settings.remove(KEY_OVERLAY_PLAYER_STATISTICS);
    settings.remove(KEY_OVERLAY_PLAYER_X);
    settings.remove(KEY_OVERLAY_PLAYER_Y);
    settings.remove(KEY_OVERLAY_PLAYER_SCALE);
    settings.remove(KEY_OVERLAY_OPPONENT_ENABLED);
    settings.remove(KEY_OVERLAY_OPPONENT_X);
    settings.remove(KEY_OVERLAY_OPPONENT_Y);
    settings.remove(KEY_OVERLAY_OPPONENT_SCALE);
    settings.remove(KEY_OVERLAY_DRAFT_ENABLED);
    settings.remove(KEY_OVERLAY_DRAFT_X);
    settings.remove(KEY_OVERLAY_DRAFT_Y);
    settings.remove(KEY_OVERLAY_DRAFT_SCALE);
}
