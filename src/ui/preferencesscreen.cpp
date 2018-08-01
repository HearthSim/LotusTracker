#include "preferencesscreen.h"
#include "ui_preferences.h"
#include "../macros.h"

PreferencesScreen::PreferencesScreen(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::Preferences())
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle( qApp->applicationName() );

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);

    ui->actionGeneral->setActionGroup(group);
    ui->actionGeneral->setProperty("pageIndex", 0);
    ui->actionOverlay->setActionGroup(group);
    ui->actionOverlay->setProperty("pageIndex", 1);
    ui->actionLogs->setActionGroup(group);
    ui->actionLogs->setProperty("pageIndex", 2);
    ui->actionAbout->setActionGroup(group);
    ui->actionAbout->setProperty("pageIndex", 3);

    tabGeneral = new TabGeneral(this);
    tabOverlay = new TabOverlay(this);
    tabLogs = new TabLogs(this);
    tabAbout = new TabAbout(this);

    tabs[0] = tabGeneral;
    tabs[1] = tabOverlay;
    tabs[2] = tabLogs;
    tabs[3] = tabAbout;

    QLayout *layout = ui->preferencesWidget->layout();
    for( int i = 0; i < NUM_TABS; i++ ) {
        layout->addWidget(tabs[i]);
    }

    connect(group, &QActionGroup::triggered,
            this, &PreferencesScreen::tabClick );
    ui->actionGeneral->trigger();
}

PreferencesScreen::~PreferencesScreen()
{
    delete ui;
}

void PreferencesScreen::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

TabGeneral *PreferencesScreen::getTabGeneral()
{
    return tabGeneral;
}

TabOverlay *PreferencesScreen::getTabOverlay()
{
    return tabOverlay;
}

TabLogs *PreferencesScreen::getTabLogs()
{
    return tabLogs;
}

TabAbout *PreferencesScreen::getTabAbout()
{
    return tabAbout;
}

void PreferencesScreen::tabClick(QAction *action) {
    int pageIndex = action->property("pageIndex").toInt();
    for (int i = 0; i<NUM_TABS; i++) {
        tabs[i]->hide();
    }
    ui->statusBar->hide();
    if (tabs[pageIndex] == tabGeneral || tabs[pageIndex] == tabOverlay) {
        ui->statusBar->show();
    }
    tabs[pageIndex]->show();
    resize(0, 0);
    adjustSize();
}
