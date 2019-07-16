#include "untapped.h"
#include "../macros.h"

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>

#define TAR_PATH "bin/tar.exe"
#define XZ_PATH "bin/xz.exe"

Untapped::Untapped(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    tempDir = QFile(dataDir + QDir::separator() + "temp").fileName();
    QDir dir(tempDir);
    if (!dir.exists() || dir.isEmpty()) {
        QDir dir;
        dir.mkpath(tempDir);
    }

    untappedAPI = new UntappedAPI(this);
    setupUntappedAPIConnections();
}

void Untapped::setupUntappedAPIConnections()
{
    connect(untappedAPI, &UntappedAPI::sgnS3PutInfo, this, &Untapped::onS3PutInfo);
    connect(untappedAPI, &UntappedAPI::sgnNewAnonymousUploadToken,
            this, [](QString uploadToken){
        APP_SETTINGS->setUntappedAnonymousUploadToken(uploadToken);
    });
}

void Untapped::checkForUntappedUploadToken()
{
    if (APP_SETTINGS->getUntappedAnonymousUploadToken().isEmpty()) {
        untappedAPI->fetchAnonymousUploadToken();
    }
}

void Untapped::setEventPlayerCourse(EventPlayerCourse eventPlayerCourse)
{
    this->eventPlayerCourse = eventPlayerCourse;
}

void Untapped::uploadMatchToUntapped(MatchInfo matchInfo, QStack<QString> matchLogMsgs)
{
    this->matchInfo = matchInfo;
    preparedMatchLogFile(matchLogMsgs);
    untappedAPI->requestS3PutUrl();
}

void Untapped::preparedMatchLogFile(QStack<QString> matchLogMsgs)
{
    QFile logFile(tempDir + QDir::separator() + "log.txt");
    if (logFile.exists()) {
      logFile.remove();
    }
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);
    logFile.seek(0);
    while(!matchLogMsgs.isEmpty()) {
        logFile.write(matchLogMsgs.pop().toUtf8());
    }
    logFile.flush();
    logFile.close();
    QProcess::execute(XZ_PATH, QStringList() << "-f" << logFile.fileName());
}

void Untapped::preparedMatchDescriptor(QString timestamp)
{
    if (matchInfo.games.isEmpty()) {
        influx_metric(influxdb_cpp::builder()
            .meas("lt_match_without_games")
            .tag("matchId", matchInfo.matchId.toStdString())
            .tag("event", matchInfo.eventId.toStdString())
            .field("count", 1)
        );
        return;
    }
    QJsonDocument descriptor(
        QJsonObject({
            { "timestamp", timestamp },
            { "summarizedMessageCount", matchInfo.summarizedMessage },
            { "client", QString("lt-%1").arg(qApp->applicationVersion()) },
            { "mtgaVersion", LOTUS_TRACKER->mtgArena->getClientVersion() },
            { "upload_token", APP_SETTINGS->getUntappedAnonymousUploadToken() },
            { "match", QJsonObject({
                { "matchId", matchInfo.matchId },
                { "deck", deckToJsonObject(matchInfo.games[0].playerDeck) },
                { "games", getMatchGamesDescriptor() },
                { "player", getMatchPlayerDescriptor() },
                { "opponents", getMatchOpponentsDescriptor() },
                { "result", resultSpecToJsonObject(matchInfo.resultSpec) },
                { "seasonOrdinal", matchInfo.seasonOrdinal }
            })},
            { "event", getMatchEventDescriptor() }
        })
    );
    QFile descriptorFile(tempDir + QDir::separator() + "descriptor.json");
    if (descriptorFile.exists()) {
      descriptorFile.remove();
    }
    descriptorFile.open(QIODevice::WriteOnly | QIODevice::Text);
    descriptorFile.write(descriptor.toJson());
    descriptorFile.flush();
    descriptorFile.close();
}

QJsonArray Untapped::getMatchGamesDescriptor()
{
    QJsonArray games;
    for(GameInfo game : matchInfo.games) {
        games.append(QJsonObject({
            { "deck", deckToJsonObject(game.playerDeck) },
            { "duration", game.duration },
            { "opponentRevealedCards", cardsToJsonArray(game.opponentRevealedDeck.cards()) },
            { "result", resultSpecToJsonObject(game.resultSpec) }
        }));
    }
    return games;
}

QJsonObject Untapped::getMatchPlayerDescriptor()
{
    return QJsonObject({
       { "name", matchInfo.player.name() },
       { "accountId", matchInfo.player.accountId() },
       { "teamId", matchInfo.player.teamId() },
       { "systemSeatId", matchInfo.player.seatId() },
       { "preMatchRankInfo", QJsonObject({
           { "rankClass", matchInfo.playerCurrentRankInfo.rankClass() },
           { "tier", intToJsonValue(matchInfo.playerCurrentRankInfo.rankTier()) },
           { "step", intToJsonValue(matchInfo.playerCurrentRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intToJsonValue(matchInfo.playerCurrentRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleToJsonValue(matchInfo.playerCurrentRankInfo.mythicPercentile()) }
       })},
       { "postMatchRankInfo", QJsonObject({
           { "rankClass", matchInfo.playerOldRankInfo.rankClass() },
           { "tier", intToJsonValue(matchInfo.playerOldRankInfo.rankTier()) },
           { "step", intToJsonValue(matchInfo.playerOldRankInfo.rankStep()) },
           { "mythicLeaderboardPlace", intToJsonValue(matchInfo.playerOldRankInfo.mythicLeaderboardPlace()) },
           { "mythicPercentile", doubleToJsonValue(matchInfo.playerOldRankInfo.mythicPercentile()) }
       })}
    });
}

QJsonArray Untapped::getMatchOpponentsDescriptor()
{
    return QJsonArray({
        QJsonObject({
            { "name", matchInfo.opponent.name() },
            { "accountId", matchInfo.opponent.accountId() },
            { "teamId", matchInfo.opponent.teamId() },
            { "systemSeatId", matchInfo.opponent.seatId() },
            { "preMatchRankInfo", QJsonObject({
                { "rankClass", matchInfo.opponentRankInfo.rankClass() },
                { "tier", intToJsonValue(matchInfo.opponentRankInfo.rankTier()) },
                { "step", intToJsonValue(matchInfo.opponentRankInfo.rankStep()) },
                { "mythicLeaderboardPlace", matchInfo.opponentRankInfo.mythicLeaderboardPlace() },
                { "mythicPercentile", matchInfo.opponentRankInfo.mythicPercentile() }
            })},
            { "postMatchRankInfo", QJsonValue::Null }
        })
    });
}

QJsonObject Untapped::getMatchEventDescriptor()
{
    return QJsonObject({
       { "name", matchInfo.eventId },
       { "maxWins", eventCourseIntToJsonValue(eventPlayerCourse.maxWins) },
       { "maxLosses", eventCourseIntToJsonValue(eventPlayerCourse.maxLosses) },
       { "currentWins", eventCourseIntToJsonValue(eventPlayerCourse.currentWins) },
       { "currentLosses", eventCourseIntToJsonValue(eventPlayerCourse.currentLosses) },
       { "processedMatchIds", eventPlayerCourse.eventId == matchInfo.eventId
         ? eventPlayerCourse.processedMatchIds : QJsonArray() },
     });
}

QJsonArray Untapped::cardsToJsonArray(QMap<Card *, int> cards)
{
    QJsonArray cardsArray;
    for (Card* card : cards.keys()) {
        for(int i=0; i<cards[card]; i++) {
            cardsArray.append(card->mtgaId);
        }
    }
    return cardsArray;
}

QJsonObject Untapped::deckToJsonObject(Deck deck)
{
    QJsonArray cardSkins;
    for (QPair<int, QString> cardSkin : deck.cardSkins) {
        cardSkins.append(QJsonObject({
            { "grpId", cardSkin.first },
            { "ccv", cardSkin.second }
        }));
    }
    return QJsonObject({
       { "mainDeck", cardsToJsonArray(deck.cards()) },
       { "sideboard", cardsToJsonArray(deck.sideboard()) },
       { "name", deck.name },
       { "boxId", deck.id },
       { "deckTileId", deck.deckTileId },
       { "cardSkins", cardSkins }
    });
}

QJsonObject Untapped::resultSpecToJsonObject(ResultSpec resultSpec)
{
    QJsonObject resultJson({
        { "scope", resultSpec.scope },
        { "result", resultSpec.result },
        { "winningTeamId", resultSpec.winningTeamId }
    });
    if (!resultSpec.reason.isEmpty()) {
        resultJson.insert("reason", resultSpec.reason);
    }
    return resultJson;
}

QJsonValue Untapped::eventCourseIntToJsonValue(int value)
{
    if (eventPlayerCourse.eventId != matchInfo.eventId) {
        return QJsonValue::Null;
    }
    return intToJsonValue(value);
}

QJsonValue Untapped::intToJsonValue(int value)
{
    if (value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}

QJsonValue Untapped::doubleToJsonValue(double value)
{
    if (value < 0) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}

void Untapped::onS3PutInfo(QString putUrl, QString timestamp)
{
    preparedMatchDescriptor(timestamp);
    preparedPutPayloadFile();
    LOGI("Upload");
}

void Untapped::preparedPutPayloadFile()
{
    QFile packedFile(tempDir + QDir::separator() + "match.packed");
    if (packedFile.exists()) {
      packedFile.remove();
    }
    QStringList args;
    args << "-cf";
    args << packedFile.fileName();
    args << "-C";
    args << tempDir;
    args << "descriptor.json";
    args << "log.txt.xz";
    QProcess::execute(TAR_PATH, args);
    QFile(tempDir + QDir::separator() + "descriptor.json").remove();
    QFile(tempDir + QDir::separator() + "log.txt.xz").remove();
}
