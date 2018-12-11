#ifndef TRAYICON_H
#define TRAYICON_H

#include "../entity/user.h"

#include <QAction>
#include <QObject>
#include <QSystemTrayIcon>

class TrayIcon : public QObject
{
    Q_OBJECT

private:
    QSystemTrayIcon *trayIcon;
    QAction *signAction;
    QAction *profileAction;
    QAction *logoutAction;
    void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setupTrayIcon();
    void configTestMenu(QMenu* testMenu);

public:
    explicit TrayIcon(QObject *parent = nullptr);
    ~TrayIcon();
    void showMessage(QString title, QString msg);
    void updateUserSettings();

signals:

private slots:
    void signIn();
    void openProfile();
    void openPreferences();
    void signOut();

public slots:
};

#endif // TRAYICON_H
