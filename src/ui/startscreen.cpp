#include "startscreen.h"
#include "../macros.h"
#include "ui_start.h"

#include <QFontDatabase>

StartScreen::StartScreen(QWidget *parent) : QMainWindow(parent),
  ui(new Ui::Start()), auth(new Auth(this))
{
  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);

  int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
  QFont trackerFont = ui->lbTracker->font();
  trackerFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
  ui->lbTracker->setFont(trackerFont);
  ui->frameLogin->setVisible(false);
  ui->frameNew->setVisible(false);

  ui->edLoginPassword->setEchoMode(QLineEdit::Password);
  ui->edLoginPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
  ui->edNewPassword->setEchoMode(QLineEdit::Password);
  ui->edNewPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
  ui->edNewConfirm->setEchoMode(QLineEdit::Password);
  ui->edNewConfirm->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

  connect(ui->lbClose, &QPushButton::clicked, qApp, &QCoreApplication::quit);
  connect(ui->btLogin, &QPushButton::clicked, this, &StartScreen::onLoginClick);
  connect(ui->btNew, &QPushButton::clicked, this, &StartScreen::onNewUserClick);
  connect(ui->btEnter, &QPushButton::clicked, this, &StartScreen::onEnterClick);
  connect(ui->btRegister, &QPushButton::clicked, this, &StartScreen::onRegisterClick);
}

StartScreen::~StartScreen()
{
    DEL(ui);
    DEL(auth);
}

void StartScreen::onLoginClick()
{
    ui->frameLogin->setVisible(true);
    ui->btLogin->setVisible(false);
    ui->btNew->setVisible(false);
}

void StartScreen::onNewUserClick()
{
    ui->frameNew->setVisible(true);
    ui->btLogin->setVisible(false);
    ui->btNew->setVisible(false);
}

void StartScreen::onEnterClick()
{

}

void StartScreen::onRegisterClick()
{
    QString email = ui->edNewEmail->text();
    QString pass = ui->edNewPassword->text();
    QString confirm = ui->edNewConfirm->text();
    if (pass == confirm) {
        auth->registerUser(email, pass);
    } else {
        ARENA_TRACKER->showMessage("", tr("Password and Confirm must be equals."));
    }
}
