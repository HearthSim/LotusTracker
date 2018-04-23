#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Preferences())
{
    ui->setupUi(this);
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
