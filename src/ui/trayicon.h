#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>

class TrayIcon : public QObject
{
    Q_OBJECT

private:
    QSystemTrayIcon *trayIcon;
    void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setupTrayIcon();
    void openPreferences();
    void configTestMenu(QMenu* testMenu);

public:
    explicit TrayIcon(QObject *parent = nullptr);
    ~TrayIcon();

signals:

public slots:
};

#endif // TRAYICON_H
