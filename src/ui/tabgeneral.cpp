#include "tabgeneral.h"
#include "ui_tabgeneral.h"
#include "../macros.h"

#if defined Q_OS_MAC
#include "../utils/macautostart.h"
#elif defined Q_OS_WIN
#include "../utils/winautostart.h"
#endif

#include <QDir>
#include <QFileDialog>
#include <QToolTip>
#include <QStandardPaths>

#define LOG_PATH QString("AppData%1LocalLow%2Wizards Of The Coast%3MTGA")\
    .arg(QDir::separator()).arg(QDir::separator()).arg(QDir::separator())

#if defined Q_OS_MAC
#define LOG_FILE_FILTER "Magic Arena app (*.app);;Arena Log (output_log.txt)"
#elif defined Q_OS_WIN
#define LOG_FILE_FILTER "Arena Log (output_log.txt)"
#endif

TabGeneral::TabGeneral(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabGeneral)
{
    ui->setupUi(this);
    applyCurrentSettings();
    connect(ui->btCheckUpdate, &QPushButton::clicked, this, [] {
        LOTUS_TRACKER->sparkleUpdater->CheckForUpdatesNow();
    });
    connect(ui->btOpenLog, &QPushButton::clicked, this, [this] {
        QString file = QFileDialog::getOpenFileName(this,
                tr("Open magic arena log"), "", tr(LOG_FILE_FILTER));
        if (file.isEmpty())
            return;
        else {
            QString logFileName = QString("%1output_log.txt").arg(QDir::separator());
            if (file.endsWith(logFileName)) {
                file = file.replace(logFileName, "");
            }
            if (file.endsWith(".app")) {
                QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
                QString userName = homeDir.right(homeDir.length() - homeDir.lastIndexOf(QDir::separator()) - 1);
                file = file + QDir::separator() + "Contents" + QDir::separator() + "Resources" +
                        QDir::separator() + "drive_c" + QDir::separator() + "users" +
                        QDir::separator() + userName + QDir::separator() + LOG_PATH;
            }
            LOTUS_TRACKER->appSettings->setLogPath(file);
            ui->leLog->setText(file);
            emit sgnLogFilePathChanged(file);
        }
    });
    connect(ui->cbStartAtLogin, &QCheckBox::clicked,
            this, &TabGeneral::onStartAtLoginChanged);
    connect(ui->cbAutoUpdate, &QCheckBox::clicked,
            this, &TabGeneral::onAutoUpdateChanged);
    connect(ui->cbPOEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onPOEnabledChanged);
    connect(ui->cbOOEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onOOEnabledChanged);
    connect(ui->cbSDADEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onSDADEnabledChanged);
    connect(ui->cbDOEnabled, &QCheckBox::clicked,
            this, &TabGeneral::onDOEnabledChanged);
    connect(ui->rbRankLSV, &QCheckBox::clicked,
            this, &TabGeneral::onRBRankChanged);
    connect(ui->rbRankDraftsim, &QCheckBox::clicked,
            this, &TabGeneral::onRBRankChanged);
    connect(ui->cbHideOnLoseGameFocus, &QCheckBox::clicked,
            this, &TabGeneral::onHideOnLoseGameFocusChanged);
    connect(ui->btReset, &QPushButton::clicked,
            this, &TabGeneral::onRestoreDefaultsSettingsClicked);
}

TabGeneral::~TabGeneral()
{
    DEL(ui)
}

void TabGeneral::onSwitchDraftRatingsSource()
{
    QString draftSource = "lsv";
    if (APP_SETTINGS->getDeckOverlayDraftSource() == "lsv") {
        draftSource = "draftsim";
    }
    emit sgnDraftOverlaySource(draftSource);
    LOGD(QString("DraftOverlaySource: %1").arg(draftSource));
    APP_SETTINGS->setDeckOverlayDraftSource(draftSource);
}

void TabGeneral::applyCurrentSettings()
{
    ui->leLog->setText(LOTUS_TRACKER->appSettings->getLogPath());
    ui->cbStartAtLogin->setChecked(APP_SETTINGS->isAutoStartEnabled());
    ui->btCheckUpdate->setChecked(LOTUS_TRACKER->sparkleUpdater->AutomaticallyChecksForUpdates());
    ui->cbPOEnabled->setChecked(APP_SETTINGS->isDeckOverlayPlayerEnabled());
    ui->cbOOEnabled->setChecked(APP_SETTINGS->isDeckOverlayOpponentEnabled());
    ui->cbSDADEnabled->setChecked(APP_SETTINGS->isShowDeckAfterDraftEnabled());
    ui->cbDOEnabled->setChecked(APP_SETTINGS->isDeckOverlayDraftEnabled());
    if (APP_SETTINGS->getDeckOverlayDraftSource() == "lsv") {
        ui->rbRankLSV->setChecked(true);
    } else {
        ui->rbRankDraftsim->setChecked(true);
    }
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

void TabGeneral::onRBRankChanged()
{
    QString draftSource = "lsv";
    if (ui->rbRankDraftsim->isChecked()) {
        draftSource = "draftsim";
    }
    emit sgnDraftOverlaySource(draftSource);
    LOGD(QString("DraftOverlaySource: %1").arg(draftSource));
    APP_SETTINGS->setDeckOverlayDraftSource(draftSource);
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

void TabGeneral::onSDADEnabledChanged()
{
    bool enabled = ui->cbSDADEnabled->isChecked();
    LOGD(QString("ShowDeckAfterDraft: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableShowDeckAfterDraft(enabled);
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
