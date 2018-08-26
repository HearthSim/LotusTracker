#include "taboverlay.h"
#include "ui_TabOverlay.h"
#include "../macros.h"

TabOverlay::TabOverlay(QWidget *parent)
    : QWidget(parent), ui(new Ui::TabOverlay)
{
    ui->setupUi(this);
    applyCurrentSettings();
    connect(ui->rbMTG, &QRadioButton::clicked,
            this, &TabOverlay::onCardLayoutChanged);
    connect(ui->rbMTGA, &QRadioButton::clicked,
            this, &TabOverlay::onCardLayoutChanged);
    connect(ui->cbShowCardOnHover, &QCheckBox::clicked,
            this, &TabOverlay::onShowCardOnHoverChanged);
    connect(ui->cbShowOnlyRemainingCard, &QCheckBox::clicked,
            this, &TabOverlay::onShowOnlyRemainingCardsChanged);
    connect(ui->hsAlpha, &QSlider::valueChanged,
            this, &TabOverlay::onTrackerAlphaChanged);
    connect(ui->hsUnhideDelay, &QSlider::valueChanged,
            this, &TabOverlay::onUnhideDelayChanged);
    connect(ui->cbPTStatistics, &QCheckBox::clicked,
            this, &TabOverlay::onPTStatisticsChanged);
}

TabOverlay::~TabOverlay()
{
    DEL(ui)
}

void TabOverlay::applyCurrentSettings()
{
    if (APP_SETTINGS->getCardLayout() == "mtg") {
        ui->rbMTG->setChecked(true);
    } else {
        ui->rbMTGA->setChecked(true);
    }
    ui->hsAlpha->setValue(APP_SETTINGS->getDeckTrackerAlpha());
    int delay = APP_SETTINGS->getUnhiddenDelay();
    ui->hsUnhideDelay->setValue(delay);
    QString unhideText = delay == 0 ? tr("(disabled)")
                                    : QString(tr("(%1 seconds)")).arg(delay);
    ui->lbUnhideDelay->setText(unhideText);
    ui->cbShowCardOnHover->setChecked(APP_SETTINGS->isShowCardOnHoverEnabled());
    ui->cbShowOnlyRemainingCard->setChecked(APP_SETTINGS->isShowOnlyRemainingCardsEnabled());
    ui->cbPTStatistics->setChecked(APP_SETTINGS->isDeckTrackerPlayerStatisticsEnabled());
}

void TabOverlay::onCardLayoutChanged()
{
    QString cardLayout = "mtga";
    if (ui->rbMTG->isChecked()) {
        cardLayout = "mtg";
    }
    emit sgnTrackerCardLayout(cardLayout);
    LOGD(QString("CardLayout: %1").arg(cardLayout));
    APP_SETTINGS->setCardLayout(cardLayout);
}

void TabOverlay::onShowCardOnHoverChanged()
{
    bool enabled = ui->cbShowCardOnHover->isChecked();
    emit sgnShowCardOnHoverEnabled(enabled);
    LOGD(QString("ShowCardOnHoverEnabled: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableShowCardOnHover(enabled);
}

void TabOverlay::onShowOnlyRemainingCardsChanged()
{
    bool enabled = ui->cbShowOnlyRemainingCard->isChecked();
    emit sgnShowOnlyRemainingCardsEnabled(enabled);
    LOGD(QString("ShowOnlyRemainingCardsEnabled: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableShowOnlyRemainingCards(enabled);
}

void TabOverlay::onTrackerAlphaChanged()
{
    int alpha = ui->hsAlpha->value();
    emit sgnTrackerAlpha(alpha);
    LOGD(QString("Alpha: %1").arg(alpha));
    APP_SETTINGS->setDeckTrackerAlpha(alpha);
}

void TabOverlay::onUnhideDelayChanged()
{
    int delay = ui->hsUnhideDelay->value();
    QString unhideText = delay == 0 ? tr("(disabled)")
                                    : QString(tr("(%1 seconds)")).arg(delay);
    ui->lbUnhideDelay->setText(unhideText);
    emit sgnUnhideDelay(delay);
    LOGD(QString("UnhideDelay: %1").arg(delay));
    APP_SETTINGS->setUnhiddenDelay(delay);
}

void TabOverlay::onPTStatisticsChanged()
{
    bool enabled = ui->cbPTStatistics->isChecked();
    emit sgnPlayerTrackerStatistics(enabled);
    LOGD(QString("PlayerTrackerStatistics: %1").arg(enabled ? "true" : "false"));
    APP_SETTINGS->enableDeckTrackerPlayerStatistics(enabled);
}

void TabOverlay::onRestoreDefaultsSettings()
{
    onCardLayoutChanged();
    onTrackerAlphaChanged();
    onPTStatisticsChanged();
}
