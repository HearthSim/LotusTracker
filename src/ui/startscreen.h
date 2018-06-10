#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "../firebase/auth.h"

#include <QMainWindow>

namespace Ui { class Start; }

class StartScreen : public QMainWindow
{
    Q_OBJECT
private:
    Ui::Start *ui;
    Auth *auth;
    void onLoginClick();
    void onNewUserClick();
    void onEnterClick();
    void onRegisterClick();

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen();

signals:

public slots:
};

#endif // STARTSCREEN_H
