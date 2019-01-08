#include "tabgeneral.h"
#include "ui_tabgeneral.h"
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
        LOTUS_TRACKER->sparkleUpdater->CheckForUpdatesNow();
    });
    connect(ui->cbStartAtLogin, &QCheckBox::clicked,
            this, &TabGeneral::onStartAtLoginChanged);
    connect(ui->cbAutoUpdate, &QCheckBox::clicked,
            this, &TabGeneral::onAutoUpdateChanged);
    connect(ui->cbDOEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onDOEnabledChanged);
    connect(ui->cbPOEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onPOEnabledChanged);
    connect(ui->cbOOEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onOOEnabledChanged);
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
    ui->btCheckUpdate->setChecked(LOTUS_TRACKER->sparkleUpdater->AutomaticallyChecksForUpdates());
    ui->cbDOEnabled->setChecked(APP_SETTINGS->isDeckOverlayDraftEnabled());
    ui->cbPOEnabled->setChecked(APP_SETTINGS->isDeckOverlayPlayerEnabled());
    ui->cbOOEnabled->setChecked(APP_SETTINGS->isDeckOverlayOpponentEnabled());
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
    LOTUS_TRACKER->sparkleUpdater->SetAutomaticallyChecksForUpdates(enabled);
    LOGD(QString("AutoUpdate: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableAutoUpdate(enabled);
}

void TabGeneral::onDOEnabledChanged()
{
    bool enabled = ui->cbDOEnabled->isChecked();
    emit sgnDeckOverlayDraftEnabled(enabled);
    LOGD(QString("DeckOverlayDraftEnabled: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckOverlayDraft(enabled);
}

void TabGeneral::onPOEnabledChanged()
{
    bool enabled = ui->cbPOEnabled->isChecked();
    emit sgnPlayerOverlayEnabled(enabled);
    LOGD(QString("PlayerOverlayEnabled: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckOverlayPlayer(enabled);
}

void TabGeneral::onOOEnabledChanged()
{
    bool enabled = ui->cbOOEnabled->isChecked();
    emit sgnOpponentOverlayEnabled(enabled);
    LOGD(QString("DeckOverlayOpponent: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckOverlayOpponent(enabled);
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
    onPOEnabledChanged();
    onPOEnabledChanged();
    onOOEnabledChanged();
    emit sgnRestoreDefaults();
}
