#include "decktrackerplayer.h"
#include "../macros.h"
#include "urls.h"

#include <QAction>
#include <QClipboard>
#include <QCursor>
#include <QDesktopServices>
#include <QFontDatabase>
#include <QToolTip>

DeckTrackerPlayer::DeckTrackerPlayer(QWidget *parent) : DeckTrackerBase(parent),
    deckMenu(new QMenu()), publishingDeckIcon(":res/publish_deck.png"),
    eventName("-"), deckWins(0), deckLosses(0)
{
    deckProfileAction = new QAction(tr("Deck Profile"), this);
    connect(deckProfileAction, &QAction::triggered, this, [this](){
        UserSettings userSettings = APP_SETTINGS->getUserSettings();
        QString deckLink = QString("%1/user/decks/%2?localId=%3&email=%4&idToken=%5&refreshToken=%6&userName=%7&expiresIn=%8")
                .arg(URLs::SITE()).arg(deck.id).arg(userSettings.userId).arg(userSettings.userEmail)
                .arg(userSettings.userToken).arg(userSettings.refreshToken).arg(userSettings.getUserName())
                .arg(userSettings.expiresTokenEpoch);
        QDesktopServices::openUrl(QUrl(deckLink));
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Deck Profile");
    });
    deckMenu->addAction(deckProfileAction);
    deckPublicProfileAction = new QAction(tr("Deck Public Page"), this);
    connect(deckPublicProfileAction, &QAction::triggered, this, [this](){
        QString deckStartId = deck.id.left(deck.id.indexOf('-'));
        QString deckAlias = QString("%1-%2").arg(deckStartId).arg(deck.name);
        QString deckLink = QString("%1/decks/%2").arg(URLs::SITE()).arg(deckAlias);
        QDesktopServices::openUrl(QUrl(deckLink));
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Deck Public Page");
    });
    deckMenu->addAction(deckPublicProfileAction);
    QAction *settingsAction = new QAction(tr("Preferences"), this);
    connect(settingsAction, &QAction::triggered, this, [this](){
        hideCardOnHover();
        LOTUS_TRACKER->showPreferencesScreen();
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Preferences");
    });
    deckMenu->addAction(settingsAction);
    publishDeckTimer = new QTimer(this);
    publishDeckTimer->setInterval(250);
    connect(publishDeckTimer, &QTimer::timeout, this, &DeckTrackerPlayer::publishingDeckAnim);
    applyCurrentSettings();
    // Statistics
    int statisticsFontSize = 8;
    winrateFontSize = 8;
#if defined Q_OS_MAC
    statisticsFontSize += 2;
    winrateFontSize += 2;
#endif
    int belerenID = QFontDatabase::addApplicationFont(":/res/fonts/Beleren-Bold.ttf");
    statisticsFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    statisticsFont.setPointSize(statisticsFontSize);
    statisticsPen = QPen(Qt::white);
    winRateFont.setFamily(QFontDatabase::applicationFontFamilies(belerenID).at(0));
    winRateFont.setPointSize(winrateFontSize);
    winRatePen = QPen(Qt::white);
}

DeckTrackerPlayer::~DeckTrackerPlayer()
{

}

void DeckTrackerPlayer::onLotusAPIRequestFinishedWithSuccess()
{
    if (publishDeckTimer->isActive()) {
        stopPublishDeckAnimation();
        QString deckStartId = deck.id.left(deck.id.indexOf('-'));
        QString deckAlias = QString("%1-%2").arg(deckStartId).arg(deck.name);
        QString deckLink = QString("%1/decks/%2").arg(URLs::SITE()).arg(deckAlias);
        QApplication::clipboard()->setText(deckLink);
        LOTUS_TRACKER->showMessage(tr("Deck published/updated.\nDeck link copied to clipboard."));
        LOTUS_TRACKER->gaTracker->sendEvent("Overlay", "Deck Published");
    }
}

void DeckTrackerPlayer::onLotusAPIRequestFinishedWithError()
{
    if (publishDeckTimer->isActive()) {
        stopPublishDeckAnimation();
        LOTUS_TRACKER->showMessage(tr("Error while publishing/updating deck. Please try again."));
    }
}

void DeckTrackerPlayer::stopPublishDeckAnimation()
{
    publishDeckTimer->stop();
    publishingDeckIcon = ":res/publish_deck.png";
}

void DeckTrackerPlayer::onPositionChanged()
{
    if (uiPos.x() < 10) {
        return;
    }
    if (uiPos.x() > screen.width() - 10) {
        return;
    }
    APP_SETTINGS->setDeckTrackerPlayerPos(uiPos);
}

void DeckTrackerPlayer::onScaleChanged()
{
    DeckTrackerBase::onScaleChanged();
    APP_SETTINGS->setDeckTrackerPlayerScale(uiScale);
}

void DeckTrackerPlayer::afterPaintEvent(QPainter &painter)
{
    bool isUserLogged = APP_SETTINGS->getUserSettings().isUserLogged();
    deckProfileAction->setEnabled(isUserLogged);
    deckPublicProfileAction->setEnabled(isUserLogged);
    // Preferences button
    int buttonSize = 16 + static_cast<int> (uiScale * 1);
    int buttonMarginX = 3;
    int buttonMarginY = 2;
    int preferencesButtonY = uiPos.y() + buttonMarginY;
    QImage settings(":res/preferences.png");
    QImage settingsScaled = settings.scaled(buttonSize, buttonSize,
                                            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int settingsPlusX = uiPos.x() + uiWidth - buttonSize - buttonMarginX;
    painter.drawImage(settingsPlusX, preferencesButtonY, settingsScaled);
    preferencesButton = QRect(settingsPlusX, preferencesButtonY, buttonSize, buttonSize);
    // publish Button
    int publishButtonY = uiPos.y() + buttonSize + buttonMarginY + buttonMarginY;
    QImage publish(publishingDeckIcon);
    QImage publishScaled = publish.scaled(buttonSize, buttonSize,
                                          Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    int publishButtonX = uiPos.x() + uiWidth - buttonSize - buttonMarginX;
    painter.drawImage(publishButtonX, publishButtonY, publishScaled);
    publishDeckButton = QRect(publishButtonX, publishButtonY, buttonSize, buttonSize);
    // Event name and WinRate
    QString eventNameWithWinRate = eventName;
    if (deckWins > 0 || deckLosses > 0) {
        eventNameWithWinRate = QString("%1 | %2-%3 (%4%)").arg(eventName)
                .arg(deckWins).arg(deckLosses).arg(deckWinRate);
    }
    winRateFont.setPointSize(winrateFontSize + (uiScale / 2));
    int winRateOptions = Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip;
    QFontMetrics winrateMetrics(statisticsFont);
    int winrateTextHeight = winrateMetrics.ascent() - winrateMetrics.descent();
    int winRateY = static_cast<int> (uiPos.y() - titleHeight - 5.5 - (uiScale / 2));
    drawText(painter, winRateFont, winRatePen, eventNameWithWinRate, winRateOptions, true,
             uiPos.x(), winRateY, winrateTextHeight, uiWidth);
    // Statistics
    if (!hidden && isStatisticsEnabled) {
        drawStatistics(painter);
    }
}

void DeckTrackerPlayer::applyCurrentSettings()
{
    uiPos = APP_SETTINGS->getDeckTrackerPlayerPos(uiWidth);
    uiScale = APP_SETTINGS->getDeckTrackerPlayerScale();
    isStatisticsEnabled = APP_SETTINGS->isDeckTrackerPlayerStatisticsEnabled();
    DeckTrackerBase::onScaleChanged();
}

void DeckTrackerPlayer::publishingDeckAnim()
{
    if (publishingDeckIcon == ":res/publish_deck_anim7.png") {
        publishingDeckIcon = ":res/publish_deck.png";
    }
    if (publishingDeckIcon == ":res/publish_deck_anim6.png") {
        publishingDeckIcon = ":res/publish_deck_anim7.png";
    }
    if (publishingDeckIcon == ":res/publish_deck_anim5.png") {
        publishingDeckIcon = ":res/publish_deck_anim6.png";
    }
    if (publishingDeckIcon == ":res/publish_deck_anim4.png") {
        publishingDeckIcon = ":res/publish_deck_anim5.png";
    }
    if (publishingDeckIcon == ":res/publish_deck_anim3.png") {
        publishingDeckIcon = ":res/publish_deck_anim4.png";
    }
    if (publishingDeckIcon == ":res/publish_deck_anim2.png") {
        publishingDeckIcon = ":res/publish_deck_anim3.png";
    }
    if (publishingDeckIcon == ":res/publish_deck_anim1.png") {
        publishingDeckIcon = ":res/publish_deck_anim2.png";
    }
    if (publishingDeckIcon == ":res/publish_deck.png") {
        publishingDeckIcon = ":res/publish_deck_anim1.png";
    }
    update();
}

void DeckTrackerPlayer::drawStatistics(QPainter &painter)
{
    if (deck.currentCards().size() == 0) {
        return;
    }
    // Statistics BG
    QRect statisticsRect(uiPos.x(), uiPos.y() + uiHeight, uiWidth, uiWidth/4);
    painter.setPen(bgPen);
    painter.setBrush(QBrush(QColor(70, 70, 70, 175)));
    painter.drawRoundedRect(statisticsRect, cornerRadius, cornerRadius);
    // Statistics info
    QFontMetrics statisticsMetrics(statisticsFont);
    int statisticsTextHeight = statisticsMetrics.ascent() - statisticsMetrics.descent();
    int statisticsTextMargin = 5;
    // Statistics title
    int statisticsBorderMargin = 2;
    int statisticsTitleOptions = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip;
    QString statisticsTitle = QString(tr("Draw chances:"));
    int statisticsTitleX = uiPos.x() + 8;
    int statisticsTitleY = uiPos.y() + uiHeight + statisticsTextMargin;
    drawText(painter, statisticsFont, statisticsPen, statisticsTitle, statisticsTitleOptions, false,
             statisticsTitleX, statisticsTitleY, statisticsTextHeight, uiWidth - statisticsBorderMargin);
    // Statistics text
    int statisticsTextOptions = Qt::AlignCenter | Qt::AlignVCenter | Qt::TextDontClip;
    int statisticsTextX = uiPos.x() + statisticsBorderMargin;
    int statisticsText1Y = statisticsTitleY + statisticsTextHeight + statisticsTextMargin + statisticsBorderMargin;
    double totalCards = deck.totalCards();
    double drawChanceLandCards = deck.totalCardsLand() * 100 / totalCards;
    double drawChance1xCards = deck.totalCardsOfQtd(1) > 0 ? 1 * 100 / totalCards : 0;
    double drawChance2xCards = deck.totalCardsOfQtd(2) > 0 ? 2 * 100 / totalCards : 0;
    double drawChance3xCards = deck.totalCardsOfQtd(3) > 0 ? 3 * 100 / totalCards : 0;
    double drawChance4xCards = deck.totalCardsOfQtd(4) > 0 ? 4 * 100 / totalCards : 0;
    QString statisticsText1 = QString("1x: %1%    2x: %2%    3x: %3%")
            .arg(drawChance1xCards, 0, 'g', 2)
            .arg(drawChance2xCards, 0, 'g', 2)
            .arg(drawChance3xCards, 0, 'g', 2);
    drawText(painter, statisticsFont, statisticsPen, statisticsText1, statisticsTextOptions, false,
             statisticsTextX, statisticsText1Y, statisticsTextHeight, uiWidth - statisticsBorderMargin);
    int statisticsText2Y = statisticsText1Y + statisticsTextHeight + statisticsTextMargin;
    QString statisticsText2 = QString("4x: %1%    Land: %2%")
            .arg(drawChance4xCards, 0, 'g', 2)
            .arg(drawChanceLandCards, 0, 'g', 2);
    drawText(painter, statisticsFont, statisticsPen, statisticsText2, statisticsTextOptions, false,
             statisticsTextX, statisticsText2Y, statisticsTextHeight, uiWidth - statisticsBorderMargin);
    uiHeight += statisticsRect.height();
}

int DeckTrackerPlayer::getDeckNameYPosition()
{
    return uiPos.y() - (titleHeight + 7 + (uiScale / 2)) * 2;
}

QString DeckTrackerPlayer::onGetDeckColorIdentity()
{
    return deck.colorIdentity();
}

void DeckTrackerPlayer::loadDeck(Deck deck)
{
    this->deck = deck;
    this->deckWins = 0;
    this->deckLosses = 0;
    this->deckWinRate = 0;
    this->eventName = "-";
    this->deck.showOnlyRemainingCards = APP_SETTINGS->isShowOnlyRemainingCardsEnabled();
    LOGI(QString("Loading deck %1").arg(deck.name));
}

void DeckTrackerPlayer::loadDeckWithSideboard(QMap<Card*, int> cards, QMap<Card*, int> sideboard)
{
    this->deck.updateCards(cards, sideboard);
    LOGI("Deck with sideboard loaded");
}

void DeckTrackerPlayer::reset()
{
    deck.reset();
    update();
}

bool DeckTrackerPlayer::isDeckLoadedAndReseted()
{
    return deck.cards().size() > 0 && deck.isReseted();
}

void DeckTrackerPlayer::onPlayerPutInLibraryCard(Card* card)
{
    deck.insertCard(card);
}

void DeckTrackerPlayer::onPlayerDrawCard(Card* card)
{
    if (deck.drawCard(card)) {
        blinkCard(card);
    }
}

void DeckTrackerPlayer::onPlayerDeckStatus(int wins, int losses, double winRate)
{
    deckWins = wins;
    deckLosses = losses;
    deckWinRate = winRate;
    update();
}

void DeckTrackerPlayer::onReceiveEventInfo(QString name, QString type)
{
    UNUSED(type);
    eventName = name;
    update();
}

void DeckTrackerPlayer::onPlayerDiscardCard(Card* card)
{
    UNUSED(card);
}

void DeckTrackerPlayer::onPlayerDiscardFromLibraryCard(Card* card)
{
    if (deck.drawCard(card)) {
        blinkCard(card);
    }
}

void DeckTrackerPlayer::onPlayerPutOnBattlefieldCard(Card* card)
{
    if (deck.drawCard(card)) {
        blinkCard(card);
    }
}

void DeckTrackerPlayer::onShowOnlyRemainingCardsEnabled(bool enabled)
{
    deck.showOnlyRemainingCards = enabled;
    update();
}

void DeckTrackerPlayer::onStatisticsEnabled(bool enabled)
{
    isStatisticsEnabled = enabled;
    update();
}

void DeckTrackerPlayer::onHoverMove(QHoverEvent *event)
{
    showingTooltip = false;
    if (publishDeckButton.contains(event->pos())) {
        showingTooltip = true;
        QString tooltip = QString(tr("Publish/Update deck as public in %1"))
                .arg(URLs::SITE());
        QToolTip::showText(event->pos(), tooltip);
    }
    if (preferencesButton.contains(event->pos())) {
        showingTooltip = true;
        QToolTip::showText(event->pos(), tr("Settings"));
    }
    DeckTrackerBase::onHoverMove(event);
}

void DeckTrackerPlayer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }
    if (publishDeckButton.contains(event->pos())) {
        return;
    }
    if (preferencesButton.contains(event->pos())) {
        return;
    }
    DeckTrackerBase::mousePressEvent(event);
}

void DeckTrackerPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        return;
    }
    if (publishDeckButton.contains(event->pos()) && !publishDeckTimer->isActive()) {
        hideCardOnHover();
        UserSettings userSettings = APP_SETTINGS->getUserSettings();
        if (userSettings.isUserLogged()) {
            publishDeckTimer->start();
            LOTUS_TRACKER->publishOrUpdatePlayerDeck(deck);
        } else {
            LOTUS_TRACKER->showMessage(tr("User need to be logged before publish a deck."));
        }
        return;
    }
    if (preferencesButton.contains(event->pos())) {
        deckMenu->exec(QCursor::pos());
        return;
    }
    DeckTrackerBase::mouseReleaseEvent(event);
}
