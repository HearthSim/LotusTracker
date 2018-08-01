#include "tablogs.h"
#include "ui_TabLogs.h"
#include "../macros.h"

TabLogs::TabLogs(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabLogs)
{
    ui->setupUi(this);
}

TabLogs::~TabLogs()
{
    DEL(ui)
}

void TabLogs::onNewLog(LogType type, const QString &log)
{
    UNUSED(type);
    ui->teLogs->appendPlainText(log);
    ui->teLogs->moveCursor( QTextCursor::End );
}
