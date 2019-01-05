#include "tababout.h"
#include "ui_tababout.h"
#include "../macros.h"

#include <QFontDatabase>

TabAbout::TabAbout(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabAbout)
{
    ui->setupUi(this);

    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
    QFont trackerFont = ui->lbTracker->font();
    trackerFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    ui->lbTracker->setFont(trackerFont);
    ui->lbVersion->setText(qApp->applicationVersion());
}

TabAbout::~TabAbout()
{
    DEL(ui)
}
