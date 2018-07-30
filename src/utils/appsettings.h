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
    int getDeckTrackerAlpha();
    void setDeckTrackerAlpha(int alpha);
    QString getCardLayout();
    void setCardLayout(QString cardLayout);
    int getUnhiddenDelay();
    void setUnhiddenDelay(int unhiddenTimeout);
    bool isShowCardOnHoverEnabled();
    void enableShowCardOnHover(bool enabled);
    bool isShowOnlyRemainingCardsEnabled();
    void enableShowOnlyRemainingCards(bool enabled);
    // Deck tracker player
    bool isDeckTrackerPlayerEnabled();
    void enableDeckTrackerPlayer(bool enabled);
    bool isDeckTrackerPlayerStatisticsEnabled();
    void enableDeckTrackerPlayerStatistics(bool enabled);
    QPoint getDeckTrackerPlayerPos(int uiWidth);
    void setDeckTrackerPlayerPos(QPoint pos);
    qreal getDeckTrackerPlayerScale();
    void setDeckTrackerPlayerScale(qreal scale);
    // Deck tracker opponent
    bool isDeckTrackerOpponentEnabled();
    void enableDeckTrackerOpponent(bool enabled);
    QPoint getDeckTrackerOpponentPos(int uiWidth);
    void setDeckTrackerOpponentPos(QPoint pos);
    qreal getDeckTrackerOpponentScale();
    void setDeckTrackerOpponentScale(qreal scale);

    void setUserSettings(UserSettings userSettings, QString userName = "");
    UserSettings getUserSettings();
    void clearUserSettings();
    void restoreDefaults();

};

#endif // APPSETTINGS_H
