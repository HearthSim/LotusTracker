#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include "../entity/user.h"

#include <QSettings>

class AppSettings : public QObject
{
    Q_OBJECT

private:
    QSettings settings;

public:
    explicit AppSettings(QObject *parent = nullptr);

    bool isAutoStartEnabled();
    void enableAutoStart(bool enabled);
    bool isAutoUpdateEnabled();
    void enableAutoUpdate(bool enabled);
    bool isFirstRun();
    bool isFirstMatch();
    bool isHideOnLoseGameFocusEnabled();
    void enableHideOnLoseGameFocus(bool enabled);
    int getDeckTrackerAlpha();
    void setDeckTrackerAlpha(int alpha);
    QString getCardLayout();
    void setCardLayout(QString cardLayout);
    int getUnhiddenDelay();
    void setUnhiddenDelay(int unhiddenTimeout);
    bool isShowCardManaCostEnabled();
    void enableShowCardManaCost(bool enabled);
    bool isShowCardOnHoverEnabled();
    void enableShowCardOnHover(bool enabled);
    bool isShowOnlyRemainingCardsEnabled();
    void enableShowOnlyRemainingCards(bool enabled);
    bool isShowDebugLogsEnabled();
    void enableShowDebugLogs(bool enabled);
    // Deck tracker player
    bool isDeckTrackerPlayerEnabled();
    void enableDeckTrackerPlayer(bool enabled);
    bool isDeckTrackerPlayerStatisticsEnabled();
    void enableDeckTrackerPlayerStatistics(bool enabled);
    QPoint getDeckTrackerPlayerPos(int uiWidth);
    void setDeckTrackerPlayerPos(QPoint pos);
    int getDeckTrackerPlayerScale();
    void setDeckTrackerPlayerScale(int scale);
    // Deck tracker opponent
    bool isDeckTrackerOpponentEnabled();
    void enableDeckTrackerOpponent(bool enabled);
    QPoint getDeckTrackerOpponentPos(int uiWidth, int cardHoverWidth);
    void setDeckTrackerOpponentPos(QPoint pos);
    int getDeckTrackerOpponentScale();
    void setDeckTrackerOpponentScale(int scale);

    void setUserSettings(UserSettings userSettings, QString userName = "");
    UserSettings getUserSettings();
    void clearUserSettings();
    void restoreDefaults();

};

#endif // APPSETTINGS_H
