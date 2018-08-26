#include "tablogs.h"
#include "ui_TabLogs.h"
#include "../macros.h"

TabLogs::TabLogs(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabLogs)
{
    ui->setupUi(this);
    ui->cbDebug->setChecked(APP_SETTINGS->isShowDebugLogsEnabled());
    connect(ui->cbDebug, &QCheckBox::clicked,
            this, &TabLogs::onShowDebugLogsChanged);
}

TabLogs::~TabLogs()
{
    DEL(ui)
}

void TabLogs::onShowDebugLogsChanged()
{
    APP_SETTINGS->enableShowDebugLogs(ui->cbDebug->isChecked());
    ui->teLogs->clear();
    for (QPair<LogType, QString> log : logs) {
        if (log.first == DEBUG && !ui->cbDebug->isChecked()) {
            continue;
        }
        ui->teLogs->appendPlainText(log.second);
    }
    ui->teLogs->moveCursor(QTextCursor::End);
}

void TabLogs::onNewLog(LogType type, const QString &log)
{
    logs << qMakePair(type, log);
    if (type == DEBUG && !ui->cbDebug->isChecked()) {
        return;
    }
    ui->teLogs->appendPlainText(log);
    ui->teLogs->moveCursor(QTextCursor::End);
}
