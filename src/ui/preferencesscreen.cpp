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
    ui->hsAlpha->setValue(APP_SETTINGS->getDeckTrackerAlpha());
    ui->cbPTEnabled->setChecked(APP_SETTINGS->isDeckTrackerPlayerEnabled());
    ui->cbPTStatistics->setChecked(APP_SETTINGS->isDeckTrackerPlayerStatisticsEnabled());
    ui->cbOTEnabled->setChecked(APP_SETTINGS->isDeckTrackerOpponentEnabled());
    connect(ui->cbStartAtLogin, &QCheckBox::clicked, this, &PreferencesScreen::onStartAtLoginChanged);
    connect(ui->rbMTG, &QRadioButton::clicked, this, &PreferencesScreen::onCardLayoutChanged);
    connect(ui->rbMTGA, &QRadioButton::clicked, this, &PreferencesScreen::onCardLayoutChanged);
    connect(ui->hsAlpha, &QSlider::valueChanged, this, &PreferencesScreen::onTrackerAlphaChanged);
    connect(ui->cbPTEnabled, &QCheckBox::clicked, this, &PreferencesScreen::onPTEnabledChanged);
    connect(ui->cbPTStatistics, &QCheckBox::clicked, this, &PreferencesScreen::onPTStatisticsChanged);
    connect(ui->cbOTEnabled, &QCheckBox::clicked, this, &PreferencesScreen::onOTEnabledChanged);
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
    APP_SETTINGS->enableAutoStart(enabled);
}

void PreferencesScreen::onTrackerAlphaChanged()
{
    int alpha = ui->hsAlpha->value();
    emit sgnTrackerAlpha(alpha);
}

void PreferencesScreen::onCardLayoutChanged()
{
    QString cardLayout = "mtga";
    if (ui->rbMTG->isChecked()) {
        cardLayout = "mtg";
    }
    emit sgnTrackerCardLayout(cardLayout);
    LOGD(QString("CardLayout: %1").arg(cardLayout));
    APP_SETTINGS->setCardLayout(cardLayout);
}

void PreferencesScreen::onPTEnabledChanged()
{
    bool enabled = ui->cbPTEnabled->isChecked();
    emit sgnPlayerTrackerEnabled(enabled);
    LOGD(QString("PlayerTrackerEnabled: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckTrackerPlayer(enabled);
}

void PreferencesScreen::onPTStatisticsChanged()
{
    bool enabled = ui->cbPTStatistics->isChecked();
    emit sgnPlayerTrackerStatistics(enabled);
    LOGD(QString("PlayerTrackerStatistics: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckTrackerPlayerStatistics(enabled);
}

void PreferencesScreen::onOTEnabledChanged()
{
    bool enabled = ui->cbOTEnabled->isChecked();
    emit sgnOpponentTrackerEnabled(enabled);
    LOGD(QString("DeckTrackerOpponent(: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckTrackerOpponent(enabled);
}
