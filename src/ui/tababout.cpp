#include "tababout.h"
#include "ui_TabAbout.h"
#include "../macros.h"

TabAbout::TabAbout(QWidget *parent)
    : QWidget(parent), ui( new Ui::TabAbout )
{
    ui->setupUi(this);
}

TabAbout::~TabAbout()
{
    DEL(ui)
}
