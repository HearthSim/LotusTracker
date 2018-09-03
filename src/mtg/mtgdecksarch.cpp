#include "mtgdecksarch.h"
#include "../server.h"
#include "../transformations.h"
#include "../macros.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonValueRef>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>

#define DECKS_ARCH_FILENAME "decksArch.json"

MtgDecksArch::MtgDecksArch(QObject *parent) : QObject(parent), deckArchs({})
{
    dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if(RUNNING_TESTS){
        dataDir = ":res";
    }
    if (!QFile::exists(dataDir)) {
        QDir dir;
        dir.mkpath(dataDir);
    }

    loadDecksArch();
}

void MtgDecksArch::updateDecksArchitectureFromAPI(){
    LOGD(QString("Updating decks architectures"));
    downloadDecksArch();
}

void MtgDecksArch::loadDecksArch()
{
    QFile decksArchFile(dataDir + QDir::separator() + DECKS_ARCH_FILENAME);
    if (QFileInfo(decksArchFile).exists()) {
        loadDecksArchFromFile();
    } else {
        downloadDecksArch();
    }
}

void MtgDecksArch::loadDecksArchFromFile() {
    QFile decksArchFile(dataDir + QDir::separator() + DECKS_ARCH_FILENAME);
    LOGD(QString("Loading %1").arg(decksArchFile.fileName()));

    if (!QFileInfo(decksArchFile).exists()) {
        LOGW(QString("%1 not found.").arg(decksArchFile.fileName()));
        return;
    }

    bool opened = decksArchFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!opened) {
        LOGW(QString("Erro while opening %1.").arg(decksArchFile.fileName()));
        return;
    }

    QByteArray jsonData = decksArchFile.readAll();
    QJsonObject jsonDecksArchitecture = Transformations::stringToJsonObject(jsonData);
    if (jsonDecksArchitecture.empty()) {
        return;
    }
    deckArchs.clear();
    for (QString deckArchName: jsonDecksArchitecture.keys()) {
        QJsonObject jsonDeckArch = jsonDecksArchitecture[deckArchName].toObject();
        QMap<int, double> deckArchCards;
        for (QString deckArchCard : jsonDeckArch.keys()) {
            deckArchCards[deckArchCard.toInt()] = jsonDeckArch[deckArchCard].toDouble();
        }
        deckArchs[deckArchName] = deckArchCards;
    }

    LOGI(QString("Decks architectures loaded with %1 archtectures")
         .arg(jsonDecksArchitecture.count()));
}

void MtgDecksArch::downloadDecksArch()
{
    QUrl url(QString("%1/decksarch").arg(Server::API_URL()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Get Request: %1").arg(url.toString()));
    }
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished,
            this, &MtgDecksArch::downloadDecksArchOnFinish);
    LOGD(QString("Downloading decks architectures"));
}

void MtgDecksArch::downloadDecksArchOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QByteArray jsonData = reply->readAll();

    if (reply->error() == QNetworkReply::ContentNotFoundError) {
        QString setUrl = reply->url().toString();
        LOGW(QString("Error while downloading mtg card json: %1").arg(setUrl));
        return;
    }

    QJsonObject jsonDecksArch = Transformations::stringToJsonObject(jsonData);
    if (jsonDecksArch.empty()) {
        return;
    }
    LOGD(QString("Downloaded %1 bytes from decks arch json").arg(jsonData.size()));

    QFile decksArchFile(dataDir + QDir::separator() + DECKS_ARCH_FILENAME);
    decksArchFile.open(QIODevice::WriteOnly);
    decksArchFile.write(jsonData);
    decksArchFile.close();

    loadDecksArchFromFile();
}

QString MtgDecksArch::findDeckArchitecture(QMap<Card*, int> cards)
{
    QString deckArchName = "";
    double deckArchValue = 0.0;
    QString deckArchSecondaryName = "";
    double deckArchSecondaryValue = 0.0;
    for (QString archName : deckArchs.keys()) {
        QMap<int, double> archCards = deckArchs[archName];
        double archValue = getCardsArchValueForDeckArch(cards, archCards);
        if (archValue == 0.0){
            continue;
        }
        if (archValue > deckArchValue) {
            deckArchName = archName;
            deckArchValue = archValue;
        }
        if (archValue > deckArchSecondaryValue && archName != deckArchName) {
            deckArchSecondaryName = archName;
            deckArchSecondaryValue = archValue;
        }
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1: %2\n").arg(archName).arg(archValue));
        }
    }
    // Check the difference first and secondary most likely arch
    double deckArchsDiff = deckArchValue - deckArchSecondaryValue;
    double deckArchsDiffPercent = deckArchsDiff / deckArchValue * 100;
    if (deckArchsDiffPercent < 10.0) {
        // Priorize the arch with most likely lands
        QMap<Card*, int> landCards;
        for (Card* card : cards.keys()) {
            if (card->isLand) {
                landCards[card] = cards[card];
            }
        }
        QMap<int, double> archCards = deckArchs[deckArchName];
        double archLandsValue = getCardsArchValueForDeckArch(landCards, archCards);
        QMap<int, double> archSecondaryCards = deckArchs[deckArchSecondaryName];
        double archSecondaryLandsValue = getCardsArchValueForDeckArch(landCards, archSecondaryCards);
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1: %2 (Only lands)").arg(deckArchName).arg(archLandsValue));
            LOGD(QString("--- %1: %2 (Only lands)\n").arg(deckArchSecondaryName).arg(archSecondaryLandsValue));
        }
        return archSecondaryLandsValue > archLandsValue ? deckArchSecondaryName : deckArchName;
    } else {
        return deckArchName;
    }
}

double MtgDecksArch::getCardsArchValueForDeckArch(QMap<Card*, int> cards, QMap<int, double> archCards)
{
    double archValue = 0.0;
    for (Card* card : cards.keys()) {
        int cardQtd = cards[card];
        double archCardQtdAverage = archCards[card->mtgaId];
        if (archCardQtdAverage == 0.0) {
            continue;
        }
        double cardRawValue = (archCardQtdAverage < cardQtd) ?
                    archCardQtdAverage / cardQtd : cardQtd / archCardQtdAverage;
        double cardValue = cardRawValue * archCardQtdAverage;
        archValue += cardValue;
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("%1: %2 of %3 = %4 * %5 = %6").arg(card->name).arg(cardQtd)
                 .arg(archCardQtdAverage).arg(cardRawValue).arg(archCardQtdAverage).arg(cardValue));
        }
    }
    return archValue;
}
