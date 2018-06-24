#include "startscreen.h"
#include "../macros.h"
#include "ui_start.h"

#include <QFontDatabase>

#define REGEXP_RAW_EMAIL "^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$"

StartScreen::StartScreen(QWidget *parent, Auth *auth) : QMainWindow(parent),
    ui(new Ui::Start()), auth(auth), reRawEmail(QRegularExpression(REGEXP_RAW_EMAIL))
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
    QFont trackerFont = ui->lbTracker->font();
    trackerFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    ui->lbTracker->setFont(trackerFont);
    ui->lbLoading->setVisible(false);
    ui->btBack->setVisible(false);
    ui->frameLogin->setVisible(false);
    ui->frameNew->setVisible(false);

    ui->edLoginPassword->setEchoMode(QLineEdit::Password);
    ui->edLoginPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    ui->edNewPassword->setEchoMode(QLineEdit::Password);
    ui->edNewPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    ui->edNewConfirm->setEchoMode(QLineEdit::Password);
    ui->edNewConfirm->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

    connect(ui->lbClose, &QPushButton::clicked, this, &StartScreen::close);
    connect(ui->btBack, &QPushButton::clicked, this, &StartScreen::onBackClick);
    connect(ui->btLogin, &QPushButton::clicked, this, &StartScreen::onLoginClick);
    connect(ui->btNew, &QPushButton::clicked, this, &StartScreen::onNewUserClick);
    connect(ui->btEnter, &QPushButton::clicked, this, &StartScreen::onEnterClick);
    connect(ui->btRegister, &QPushButton::clicked, this, &StartScreen::onRegisterClick);
    connect(auth, &Auth::sgnRequestFinished, this, [this](){
        ui->lbLoading->setVisible(false);
        ui->btEnter->setVisible(ui->frameLogin->isVisible());
        ui->btRegister->setVisible(ui->frameNew->isVisible());
    });
}

StartScreen::~StartScreen()
{
    DEL(ui)
    DEL(auth)
}

void StartScreen::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void StartScreen::onBackClick()
{
    ui->frameLogin->setVisible(false);
    ui->frameNew->setVisible(false);
    ui->btBack->setVisible(false);
    ui->btLogin->setVisible(true);
    ui->btNew->setVisible(true);
}

void StartScreen::onLoginClick()
{
    ui->frameLogin->setVisible(true);
    ui->btLogin->setVisible(false);
    ui->btNew->setVisible(false);
    ui->btBack->setVisible(true);
}

void StartScreen::onNewUserClick()
{
    ui->frameNew->setVisible(true);
    ui->btLogin->setVisible(false);
    ui->btNew->setVisible(false);
    ui->btBack->setVisible(true);
}

void StartScreen::onEnterClick()
{
    QString email = ui->edLoginEmail->text();
    QRegularExpressionMatch emailMatch = reRawEmail.match(email);
    if (!emailMatch.hasMatch()) {
        ARENA_TRACKER->showMessage("", tr("Invalid email"));
        return;
    }
    QString pass = ui->edLoginPassword->text();
    ui->btEnter->setVisible(false);
    ui->lbLoading->setVisible(true);
    auth->signInUser(email, pass);
}

void StartScreen::onRegisterClick()
{
    QString email = ui->edNewEmail->text();
    QRegularExpressionMatch emailMatch = reRawEmail.match(email);
    if (!emailMatch.hasMatch()) {
        ARENA_TRACKER->showMessage("", tr("Invalid email"));
        return;
    }
    QString pass = ui->edNewPassword->text();
    QString confirm = ui->edNewConfirm->text();
    if (pass == confirm) {
        ui->btRegister->setVisible(false);
        ui->lbLoading->setVisible(true);
        auth->registerUser(email, pass);
    } else {
        ARENA_TRACKER->showMessage("", tr("Password and Confirm must be equals."));
    }
}
