#include "preferencesscreen.h"
#include "ui_preferences.h"
#include "../macros.h"

#if defined Q_OS_MAC
#include "../utils/macautostart.h"
#elif defined Q_OS_WIN
#include "../utils/winautostart.h"
#endif

PreferencesScreen::PreferencesScreen(QWidget *parent) : QMainWindow(parent), ui(new Ui::Preferences())
{
    ui->setupUi(this);
    ui->cbStartAtLogin->setChecked(APP_SETTINGS->isAutoStartEnabled());
    connect(ui->cbStartAtLogin, &QAbstractButton::clicked, this, &PreferencesScreen::onStartAtLoginChange);
}

PreferencesScreen::~PreferencesScreen()
{
    delete ui;
}

void PreferencesScreen::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void PreferencesScreen::onStartAtLoginChange()
{
    bool enabled = ui->cbStartAtLogin->isChecked();
#if defined Q_OS_MAC
    MacAutoStart::setEnabled(enabled);
#elif defined Q_OS_WIN
    WinAutoStart::setEnabled(enabled);
#endif
    LOGD(QString("StartAtLogin: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->setAutoStart(enabled);
}
