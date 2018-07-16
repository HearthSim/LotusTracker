#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include "../firebase/auth.h"

#include <QMainWindow>
#include <QRegularExpression>

namespace Ui { class Start; }

class StartScreen : public QMainWindow
{
    Q_OBJECT
private:
    Ui::Start *ui;
    FirebaseAuth *firebaseAuth;
    QRegularExpression reRawEmail;
    void onBackClick();
    void onLoginClick();
    void onNewUserClick();
    void onEnterClick();
    void onRegisterClick();
    void onForgotPasswordClick();
    void onRecoverPasswordClick();

public:
    explicit StartScreen(QWidget *parent = nullptr, FirebaseAuth *firebaseAuth = nullptr);
    ~StartScreen();
    void closeEvent(QCloseEvent *event);

signals:

public slots:
};

#endif // STARTSCREEN_H
