#ifndef TRAYICON_H
#define TRAYICON_H

#include "../mtg/mtgcards.h"
#include "../entity/user.h"

#include <QObject>
#include <QSystemTrayIcon>

class TrayIcon : public QObject
{
    Q_OBJECT

private:
    MtgCards *mtgCards;
    QSystemTrayIcon *trayIcon;
    QAction *signAction;
    void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setupTrayIcon();
    void configTestMenu(QMenu* testMenu);

public:
    explicit TrayIcon(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    ~TrayIcon();
    void showMessage(QString title, QString msg);
    void updateUserSettings(UserSettings userSettings);

signals:

private slots:
    void openSignIn();
    void openPreferences();

public slots:
};

#endif // TRAYICON_H
