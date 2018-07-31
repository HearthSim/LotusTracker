#include "tablogs.h"
#include "ui_TabLogs.h"
#include "../macros.h"

TabLogs::TabLogs(QWidget *parent)
    : QWidget(parent), ui( new Ui::TabLogs )
{
    ui->setupUi(this);
}

TabLogs::~TabLogs()
{
    DEL(ui)
}
