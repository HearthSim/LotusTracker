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
    void setAutoStart(bool enabled);
    QString getCardLayout();
    void setCardLayout(QString cardLayout);

};

#endif // APPSETTINGS_H
