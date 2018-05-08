#ifndef APPSETTINGS_H
#define APPSETTINGS_H

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
    QString getCardLayout();
    void setCardLayout(QString cardLayout);
    // Deck tracker player
    bool isDeckTrackerPlayerEnabled();
    void enableDeckTrackerPlayer(bool enabled);
    bool isDeckTrackerPlayerStatisticsEnabled();
    void enableDeckTrackerPlayerStatistics(bool enabled);
    QPoint getDeckTrackerPlayerPos();
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

};

#endif // APPSETTINGS_H
