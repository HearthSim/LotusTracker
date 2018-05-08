#include "preferencesscreen.h"
#include "ui_preferences.h"
#include "../macros.h"

#if defined Q_OS_MAC
#include "../utils/macautostart.h"
#elif defined Q_OS_WIN
#include "../utils/winautostart.h"
#endif

PreferencesScreen::PreferencesScreen(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::Preferences())
{
    ui->setupUi(this);
    ui->cbStartAtLogin->setChecked(APP_SETTINGS->isAutoStartEnabled());
    if (APP_SETTINGS->getCardLayout() == "mtg") {
        ui->rbMTG->setChecked(true);
    } else {
        ui->rbMTGA->setChecked(true);
    }
    connect(ui->cbStartAtLogin, &QCheckBox::clicked, this, &PreferencesScreen::onStartAtLoginChanged);
    connect(ui->rbMTG, &QRadioButton::clicked, this, &PreferencesScreen::onCardLayoutChanged);
    connect(ui->rbMTGA, &QRadioButton::clicked, this, &PreferencesScreen::onCardLayoutChanged);
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

void PreferencesScreen::onStartAtLoginChanged()
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

void PreferencesScreen::onCardLayoutChanged()
{
    QString cardLayout = "mtga";
    if (ui->rbMTG->isChecked()) {
        cardLayout = "mtg";
    }
    APP_SETTINGS->setCardLayout(cardLayout);
    emit sgnTrackerCardLayout(cardLayout);
}
