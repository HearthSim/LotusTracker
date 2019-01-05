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
    bool isDeckOverlayPlayerEnabled();
    void enableDeckOverlayPlayer(bool enabled);
    bool isDeckOverlayPlayerStatisticsEnabled();
    void enableDeckOverlayPlayerStatistics(bool enabled);
    QPoint getDeckOverlayPlayerPos(int uiWidth);
    void setDecOverlayPlayerPos(QPoint pos);
    int getDeckOverlayPlayerScale();
    void setDeckOverlayPlayerScale(int scale);
    // Deck tracker opponent
    bool isDeckOverlayrOpponentEnabled();
    void enableDeckOverlayOpponent(bool enabled);
    QPoint getDeckOverlayOpponentPos(int uiWidth, int cardHoverWidth);
    void setDeckOverlayOpponentPos(QPoint pos);
    int getDeckOverlayOpponentScale();
    void setDeckOverlayOpponentScale(int scale);

    void setUserSettings(UserSettings userSettings, QString userName = "");
    UserSettings getUserSettings();
    void clearUserSettings();
    void restoreDefaults();

};

#endif // APPSETTINGS_H
