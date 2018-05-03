#include "decktrackeroverlay.h"
#include "ui_decktrackeroverlay.h"
#include "../macros.h"
#include "../mtg/mtgcards.h"

#ifdef Q_OS_MAC
#include <objc/objc-runtime.h>
#endif

#include <QDesktopWidget>

DeckTrackerOverlay::DeckTrackerOverlay(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::DeckTracker())
{
    ui->setupUi(this);
    setupWindow();

    playerDeckTrackerUI = new DeckTrackerUI(this);
    playerDeckTrackerUI->move(10, 10);
    opponentDeckTrackerUI = new DeckTrackerUI(this);
    opponentDeckTrackerUI->move(screen.width() - opponentDeckTrackerUI->getWidth() - 10, 10);
}

DeckTrackerOverlay::~DeckTrackerOverlay()
{
    delete ui;
    DELETE(playerDeckTrackerUI);
    DELETE(opponentDeckTrackerUI);
}

void DeckTrackerOverlay::setupWindow()
{
    setWindowFlags(Qt::NoDropShadowWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
#ifdef Q_OS_MAC
    setWindowFlags(windowFlags() | Qt::Window);
    WId windowObject = this->winId();
    objc_object* nsviewObject = reinterpret_cast<objc_object*>(windowObject);
    objc_object* nsWindowObject = objc_msgSend(nsviewObject, sel_registerName("window"));
    int NSWindowCollectionBehaviorCanJoinAllSpaces = 1 << 0;
    objc_msgSend(nsWindowObject, sel_registerName("setCollectionBehavior:"), NSWindowCollectionBehaviorCanJoinAllSpaces);
#else
    setWindowFlags(windowFlags() | Qt::Tool);
#endif

    screen = QApplication::desktop()->screenGeometry();
    move(0, 0);
    resize(screen.width(), screen.height() - 100);
    show();
    hide();
}

void DeckTrackerOverlay::onPlayerDeckSelected(Deck deck)
{
    playerDeckTrackerUI->setupDeck(deck);
    opponentDeckTrackerUI->setupDeck(Deck());
    update();
    show();
}

void DeckTrackerOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    playerDeckTrackerUI->paintEvent(painter);
    opponentDeckTrackerUI->paintEvent(painter);
    painter.restore();
}

void DeckTrackerOverlay::mousePressEvent(QMouseEvent *event)
{
    if (playerDeckTrackerUI->isMouseOver(event)) {
        return playerDeckTrackerUI->mousePressEvent(event);
    }
    if (opponentDeckTrackerUI->isMouseOver(event)) {
        return opponentDeckTrackerUI->mousePressEvent(event);
    }
}

void DeckTrackerOverlay::mouseMoveEvent(QMouseEvent *event)
{
    playerDeckTrackerUI->mouseMoveEvent(event);
    opponentDeckTrackerUI->mouseMoveEvent(event);
    update();
}

void DeckTrackerOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    playerDeckTrackerUI->mouseReleaseEvent(event);
    opponentDeckTrackerUI->mouseReleaseEvent(event);
}
