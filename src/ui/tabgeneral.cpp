#include "tabgeneral.h"
#include "ui_TabGeneral.h"
#include "../macros.h"

#if defined Q_OS_MAC
#include "../utils/macautostart.h"
#elif defined Q_OS_WIN
#include "../utils/winautostart.h"
#endif

#include <QToolTip>

TabGeneral::TabGeneral(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabGeneral)
{
    ui->setupUi(this);
    applyCurrentSettings();
    connect(ui->btCheckUpdate, &QPushButton::clicked, this, [] {
        ARENA_TRACKER->sparkleUpdater->CheckForUpdatesNow();
    });
    connect(ui->cbStartAtLogin, &QCheckBox::clicked,
            this, &TabGeneral::onStartAtLoginChanged);
    connect(ui->cbAutoUpdate, &QCheckBox::clicked,
            this, &TabGeneral::onAutoUpdateChanged);
    connect(ui->cbPTEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onPTEnabledChanged);
    connect(ui->cbOTEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onOTEnabledChanged);
    connect(ui->cbHideOnLoseGameFocus, &QCheckBox::clicked,
            this, &TabGeneral::onHideOnLoseGameFocusChanged);
    connect(ui->btReset, &QPushButton::clicked,
            this, &TabGeneral::onRestoreDefaultsSettingsClicked);
}

TabGeneral::~TabGeneral()
{
    DEL(ui)
}

void TabGeneral::applyCurrentSettings()
{
    ui->cbStartAtLogin->setChecked(APP_SETTINGS->isAutoStartEnabled());
    ui->btCheckUpdate->setChecked(ARENA_TRACKER->sparkleUpdater->AutomaticallyChecksForUpdates());
    ui->cbPTEnabled->setChecked(APP_SETTINGS->isDeckTrackerPlayerEnabled());
    ui->cbOTEnabled->setChecked(APP_SETTINGS->isDeckTrackerOpponentEnabled());
    ui->cbHideOnLoseGameFocus->setChecked(APP_SETTINGS->isHideOnLoseGameFocusEnabled());
}

void TabGeneral::onStartAtLoginChanged()
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

void TabGeneral::onAutoUpdateChanged()
{
    bool enabled = ui->cbAutoUpdate->isChecked();
    ARENA_TRACKER->sparkleUpdater->SetAutomaticallyChecksForUpdates(enabled);
    LOGD(QString("AutoUpdate: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableAutoUpdate(enabled);
}

void TabGeneral::onPTEnabledChanged()
{
    bool enabled = ui->cbPTEnabled->isChecked();
    emit sgnPlayerTrackerEnabled(enabled);
    LOGD(QString("PlayerTrackerEnabled: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckTrackerPlayer(enabled);
}

void TabGeneral::onOTEnabledChanged()
{
    bool enabled = ui->cbOTEnabled->isChecked();
    emit sgnOpponentTrackerEnabled(enabled);
    LOGD(QString("DeckTrackerOpponent: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckTrackerOpponent(enabled);
}

void TabGeneral::onHideOnLoseGameFocusChanged()
{
    bool enabled = ui->cbHideOnLoseGameFocus->isChecked();
    LOGD(QString("HideOnLoseGameFocus: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableHideOnLoseGameFocus(enabled);
}

void TabGeneral::onRestoreDefaultsSettingsClicked()
{
    APP_SETTINGS->restoreDefaults();
    applyCurrentSettings();
    onStartAtLoginChanged();
    onPTEnabledChanged();
    onPTEnabledChanged();
    onOTEnabledChanged();
    emit sgnRestoreDefaults();
}
