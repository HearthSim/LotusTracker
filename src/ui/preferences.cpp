#include "preferences.h"
#include "ui_preferences.h"
#include "../macros.h"

#if defined Q_OS_MAC
#include "../utils/macautostart.h"
#elif defined Q_OS_WIN
#include "../utils/winautostart.h"
#endif

Preferences::Preferences(QWidget *parent) : QMainWindow(parent), ui(new Ui::Preferences())
{
    ui->setupUi(this);
    connect(ui->startAtLogin, &QAbstractButton::clicked, this, &Preferences::onStartAtLoginChange);
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void Preferences::onStartAtLoginChange()
{
    bool enabled = ui->startAtLogin->isChecked();
#if defined Q_OS_MAC
    MacAutoStart::setEnabled(enabled);
#elif defined Q_OS_WIN
    WinAutoStart::setEnabled(enabled);
#endif
    LOGD(QString("StartAtLogin: %1").arg(enabled ? "true" : "false"));
}
