#include "appsettings.h"
#include "../macros.h"
#include "../transformations.h"

#include <QDesktopWidget>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#define KEY_INSTALLATION_UUID "installationUuid"
#define KEY_AUTOSTART "autoStart"
#define KEY_AUTOUPDATE "autoUpdate"
#define KEY_FIRST_RUN "isFirstRun"
#define KEY_FIRST_MATCH "isFirstMatch"
#define KEY_FIRST_DRAFT "isFirstDraft"
#define KEY_HIDE_ON_LOSE_GAME_FOCUS "hideOnLoseGameFocus"
#define KEY_LOG_PATH "log_path"

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
#define KEY_OVERLAY_DRAFT_X "Tracker/draftPrefs/x"
#define KEY_OVERLAY_DRAFT_Y "Tracker/draftPrefs/y"
#define KEY_OVERLAY_DRAFT_SCALE "Tracker/draftPrefs/scale"
#define KEY_OVERLAY_DRAFT_PICKS_PREFIX "Tracker/draftPrefs/picks"
#define KEY_OVERLAY_SHOW_DECK_AFTER_DRAFT_ENABLED "Tracker/draftPrefs/showDeckAfterDraft"

// Deprecate KEY_OVERLAY_USER_*, using appSecure now
#define KEY_OVERLAY_USER_ID "Tracker/user/id"
#define KEY_OVERLAY_USER_EMAIL "Tracker/user/email"
#define KEY_OVERLAY_USER_TOKEN "Tracker/user/token"
#define KEY_OVERLAY_USER_REFRESH_TOKEN "Tracker/user/refreshToken"
#define KEY_OVERLAY_USER_EXPIRES_EPOCH "Tracker/user/tokenExpiresEpoch"
#define KEY_OVERLAY_USER_NAME "Tracker/user/name"

#define DEFAULT_OVERLAY_VIEW_X 5
#define DEFAULT_OVERLAY_VIEW_Y 60

#define KEY_SECURE_USER_SETTINGS "user"

#define LOG_PATH QString("AppData%1LocalLow%2Wizards of the Coast%3MTGA")\
    .arg(QDir::separator()).arg(QDir::separator()).arg(QDir::separator())

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{
    appSecure = new AppSecure(this);
    LOGD(QString("Settings saved in %1").arg(settings.fileName()));
}

QString AppSettings::getInstallationUuid()
{
    if (!settings.contains(KEY_INSTALLATION_UUID)) {
        settings.setValue(KEY_INSTALLATION_UUID, QUuid::createUuid().toString(QUuid::WithoutBraces));
        settings.sync();
    }

    return settings.value(KEY_INSTALLATION_UUID).toString();
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

bool AppSettings::isFirstDraft()
{
    bool isFirstDraft = settings.value(KEY_FIRST_DRAFT, true).toBool();
    if (isFirstDraft) {
        settings.setValue(KEY_FIRST_DRAFT, false);
    }
    return isFirstDraft;
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

QString AppSettings::getLogPath()
{
#if defined Q_OS_MAC
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString userName = homeDir.right(homeDir.length() - homeDir.lastIndexOf(QDir::separator()) - 1);
    QString baseLogFilePath = "/Applications/MTGArena.app/Contents/Resources/drive_c/users";
    QString defaultLogPath = baseLogFilePath + QDir::separator() + userName + QDir::separator() + LOG_PATH;
#elif defined Q_OS_WIN
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString defaultLogPath = homeDir + QDir::separator() + LOG_PATH;
#endif
    return settings.value(KEY_LOG_PATH, defaultLogPath).toString();
}

void AppSettings::setLogPath(QString logPath)
{
    settings.setValue(KEY_LOG_PATH, logPath);
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

bool AppSettings::isShowDeckAfterDraftEnabled()
{
    return settings.value(KEY_OVERLAY_SHOW_DECK_AFTER_DRAFT_ENABLED, true).toBool();
}

void AppSettings::enableShowDeckAfterDraft(bool enabled)
{
    settings.setValue(KEY_OVERLAY_SHOW_DECK_AFTER_DRAFT_ENABLED, enabled);
}

bool AppSettings::hasDraftPick(QString eventId)
{
    QString eventPickKey = QString("%1/%2/0_0_picks").arg(KEY_OVERLAY_DRAFT_PICKS_PREFIX)
            .arg(eventId);
    return settings.contains(eventPickKey);
}

void AppSettings::clearDraftPick(QString eventId)
{
    QString eventPickKey = QString("%1/%2/").arg(KEY_OVERLAY_DRAFT_PICKS_PREFIX)
            .arg(eventId);
    settings.remove(eventPickKey);
}

QString AppSettings::getDraftPicks(QString eventId, int packNumber, int pickNumber)
{
    QString eventPickKey = getDraftPickBaseKey(eventId, packNumber, pickNumber);
    return settings.value(QString("%1_picks").arg(eventPickKey), "").toString();
}

QString AppSettings::getDraftPicked(QString eventId, int packNumber, int pickNumber)
{
    QString eventPickKey = getDraftPickBaseKey(eventId, packNumber, pickNumber);
    return settings.value(QString("%1_picked").arg(eventPickKey), "").toString();
}

void AppSettings::setDraftPick(QString eventId, int packNumber, int pickNumber,
                                int pickedCard, QList<Card*> availablePicks)
{
    QString eventPickKey = getDraftPickBaseKey(eventId, packNumber, pickNumber);
    QString picks;
    for(Card* card : availablePicks){
        picks += QString("%1,").arg(card->mtgaId);
    }
    picks = picks.left(picks.length()-1);
    settings.setValue(QString("%1_picks").arg(eventPickKey), picks);
    settings.setValue(QString("%1_picked").arg(eventPickKey), pickedCard);
}

QString AppSettings::getDraftPickBaseKey(QString eventId, int packNumber, int pickNumber)
{
    return QString("%1/%2/%3_%4").arg(KEY_OVERLAY_DRAFT_PICKS_PREFIX)
            .arg(eventId).arg(packNumber).arg(pickNumber);
}

// User settings

void AppSettings::setUserSettings(UserSettings userSettings, QString userName)
{
    QJsonDocument json(
                QJsonObject({
                    { "id", userSettings.userId },
                    { "name", userName },
                    { "email", userSettings.userEmail },
                    { "token", userSettings.userToken },
                    { "refresh_token", userSettings.refreshToken },
                    { "expires_epoch", userSettings.expiresTokenEpoch }
                })
    );
    appSecure->store(KEY_SECURE_USER_SETTINGS, QString(json.toJson()));
}

UserSettings AppSettings::getUserSettings()
{
    if (!settings.value(KEY_OVERLAY_USER_ID, "").toString().isEmpty()) {
        migrateUserSettings();
    }
    QString jsonText = appSecure->restore(KEY_SECURE_USER_SETTINGS);
    QJsonObject json = Transformations::stringToJsonObject(jsonText);
    UserSettings userSettings = UserSettings(json["id"].toString(""),
                        json["email"].toString(""),
                        json["token"].toString(""),
                        json["refresh_token"].toString(""),
                        json["expires_epoch"].toDouble(0));
    userSettings.setUserName(json["name"].toString(""));
    return userSettings;
}

void AppSettings::clearUserSettings()
{
    setUserSettings(UserSettings(), "");
}

void AppSettings::migrateUserSettings()
{
    UserSettings userSettings = UserSettings(settings.value(KEY_OVERLAY_USER_ID, "").toString(),
                        settings.value(KEY_OVERLAY_USER_EMAIL, "").toString(),
                        settings.value(KEY_OVERLAY_USER_TOKEN, "").toString(),
                        settings.value(KEY_OVERLAY_USER_REFRESH_TOKEN, "").toString(),
                        settings.value(KEY_OVERLAY_USER_EXPIRES_EPOCH, 0).toLongLong());
    setUserSettings(userSettings, settings.value(KEY_OVERLAY_USER_NAME, "").toString());
    settings.remove(KEY_OVERLAY_USER_ID);
    settings.remove(KEY_OVERLAY_USER_EMAIL);
    settings.remove(KEY_OVERLAY_USER_TOKEN);
    settings.remove(KEY_OVERLAY_USER_REFRESH_TOKEN);
    settings.remove(KEY_OVERLAY_USER_EXPIRES_EPOCH);
    settings.remove(KEY_OVERLAY_USER_NAME);
    LOGD("UserSettings migrated");
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
