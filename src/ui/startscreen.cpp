#include "startscreen.h"
#include "ui_start.h"
#include "../macros.h"

#include <QFontDatabase>

#define REGEXP_RAW_EMAIL "^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$"

StartScreen::StartScreen(QWidget *parent, FirebaseAuth *auth) : QMainWindow(parent),
    ui(new Ui::Start()), firebaseAuth(auth), reRawEmail(QRegularExpression(REGEXP_RAW_EMAIL))
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
    ui->frameRecoverPassword->setVisible(false);

    ui->edLoginPassword->setEchoMode(QLineEdit::Password);
    ui->edLoginPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    ui->edNewPassword->setEchoMode(QLineEdit::Password);
    ui->edNewPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    ui->edNewConfirm->setEchoMode(QLineEdit::Password);
    ui->edNewConfirm->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

    connect(ui->lbClose, &QPushButton::clicked, this, &StartScreen::close);
    connect(ui->lbForgorPassword, &QPushButton::clicked, this, &StartScreen::onForgotPasswordClick);
    connect(ui->btBack, &QPushButton::clicked, this, &StartScreen::onBackClick);
    connect(ui->btLogin, &QPushButton::clicked, this, &StartScreen::onLoginClick);
    connect(ui->btNew, &QPushButton::clicked, this, &StartScreen::onNewUserClick);
    connect(ui->btEnter, &QPushButton::clicked, this, &StartScreen::onEnterClick);
    connect(ui->btRegister, &QPushButton::clicked, this, &StartScreen::onRegisterClick);
    connect(ui->btRecoverPassword, &QPushButton::clicked, this, &StartScreen::onRecoverPasswordClick);

    connect(ui->edLoginPassword, SIGNAL(returnPressed()),ui->btEnter,SIGNAL(clicked()));
    connect(ui->edNewConfirm, SIGNAL(returnPressed()),ui->btRegister,SIGNAL(clicked()));
    connect(ui->edRecoverPasswordEmail, SIGNAL(returnPressed()),ui->btRecoverPassword,SIGNAL(clicked()));
    // Reset loading
    connect(auth, &FirebaseAuth::sgnRequestFinished, this, [this](){
        ui->lbLoading->setVisible(false);
        ui->btEnter->setVisible(true);
        ui->btRegister->setVisible(true);
        ui->btRecoverPassword->setVisible(true);
    });
    connect(auth, &FirebaseAuth::sgnUserLogged, this, [this](){ onBackClick(); });
    connect(auth, &FirebaseAuth::sgnPasswordRecovered, this, [this](){
        onBackClick();
        ARENA_TRACKER->showMessage("", tr("Email was sent with Password Recover instructions"));
    });
}

StartScreen::~StartScreen()
{
    DEL(ui)
    DEL(firebaseAuth)
}

void StartScreen::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void StartScreen::onBackClick()
{
    if (ui->frameRecoverPassword->isVisible()) {
        ui->frameRecoverPassword->setVisible(false);
        ui->frameLogin->setVisible(true);
    } else {
        ui->frameLogin->setVisible(false);
        ui->frameNew->setVisible(false);
        ui->btBack->setVisible(false);
        ui->btLogin->setVisible(true);
        ui->btNew->setVisible(true);
    }
}

void StartScreen::onForgotPasswordClick()
{
    ui->frameLogin->setVisible(false);
    ui->frameRecoverPassword->setVisible(true);
}

void StartScreen::onLoginClick()
{
    ui->frameLogin->setVisible(true);
    ui->frameNew->setVisible(false);
    ui->frameRecoverPassword->setVisible(false);
    ui->btLogin->setVisible(false);
    ui->btNew->setVisible(false);
    ui->btBack->setVisible(true);
}

void StartScreen::onNewUserClick()
{
    ui->frameLogin->setVisible(false);
    ui->frameNew->setVisible(true);
    ui->frameRecoverPassword->setVisible(false);
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
    firebaseAuth->signInUser(email, pass);
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
        firebaseAuth->registerUser(email, pass);
    } else {
        ARENA_TRACKER->showMessage("", tr("Password and Confirm must be equals."));
    }
}

void StartScreen::onRecoverPasswordClick()
{
    QString email = ui->edRecoverPasswordEmail->text();
    QRegularExpressionMatch emailMatch = reRawEmail.match(email);
    if (!emailMatch.hasMatch()) {
        ARENA_TRACKER->showMessage("", tr("Invalid email"));
        return;
    }
    ui->btRecoverPassword->setVisible(false);
    ui->lbLoading->setVisible(true);
    firebaseAuth->recoverPassword(email);
}
