#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include "../entity/card.h"
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
    bool isFirstDraft();
    bool isHideOnLoseGameFocusEnabled();
    void enableHideOnLoseGameFocus(bool enabled);
    int getDeckTrackerAlpha();
    void setDeckTrackerAlpha(int alpha);
    QString getCardLayout();
    void setCardLayout(QString cardLayout);
    QString getLogPath();
    void setLogPath(QString logPath);
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
    // Deck overlay player
    bool isDeckOverlayPlayerEnabled();
    void enableDeckOverlayPlayer(bool enabled);
    bool isDeckOverlayPlayerStatisticsEnabled();
    void enableDeckOverlayPlayerStatistics(bool enabled);
    QPoint getDeckOverlayPlayerPos(int uiWidth);
    void setDeckOverlayPlayerPos(QPoint pos);
    int getDeckOverlayPlayerScale();
    void setDeckOverlayPlayerScale(int scale);
    // Deck overlay opponent
    bool isDeckOverlayOpponentEnabled();
    void enableDeckOverlayOpponent(bool enabled);
    QPoint getDeckOverlayOpponentPos(int uiWidth, int cardHoverWidth);
    void setDeckOverlayOpponentPos(QPoint pos);
    int getDeckOverlayOpponentScale();
    void setDeckOverlayOpponentScale(int scale);
    // Deck overlay Draft
    bool isDeckOverlayDraftEnabled();
    void enableDeckOverlayDraft(bool enabled);
    QPoint getDeckOverlayDraftPos(int uiWidth);
    void setDeckOverlayDraftPos(QPoint pos);
    int getDeckOverlayDraftScale();
    void setDeckOverlayDraftScale(int scale);
    bool isShowDeckAfterDraftEnabled();
    void enableShowDeckAfterDraft(bool enabled);
    bool hasDraftPick(QString eventId);
    void clearDraftPick(QString eventId);
    QString getDraftPicks(QString eventId, int packNumber, int pickNumber);
    QString getDraftPicked(QString eventId, int packNumber, int pickNumber);
    void setDraftPick(QString eventId, int packNumber, int pickNumber,
                       int pickedCard, QList<Card*> availablePicks);
    QString getDraftPickBaseKey(QString eventId, int packNumber, int pickNumber);
    // User settings
    void setUserSettings(UserSettings userSettings, QString userName = "");
    UserSettings getUserSettings();
    void clearUserSettings();
    void restoreDefaults();

};

#endif // APPSETTINGS_H
