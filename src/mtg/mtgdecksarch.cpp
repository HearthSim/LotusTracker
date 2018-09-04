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
    for (QString archName: jsonDecksArchitecture.keys()) {
        QJsonObject jsonArch = jsonDecksArchitecture[archName].toObject();
        int archId = jsonArch["id"].toInt();
        QString archColors = jsonArch["colors"].toString();
        QJsonObject archCards = jsonArch["cards"].toObject();
        QMap<int, int> cards;
        for (QString archCard : archCards.keys()) {
            cards[archCard.toInt()] = archCards[archCard].toInt();
        }
        deckArchs << DeckArch(archId, archName, archColors, cards);
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
    QMap<Card*, int> nonlandCards;
    for (Card* card : cards.keys()) {
        if (!card->isLand) {
            nonlandCards[card] = cards[card];
        }
    }
    DeckArch deckArchFirst;
    double deckArchFirstValue = 0.0;
    DeckArch deckArchSecondary;
    double deckArchSecondaryValue = 0.0;
    DeckArch deckArchThird;
    double deckArchThirdValue = 0.0;
    for (DeckArch deckArch : deckArchs) {
        QMap<int, int> archCards = deckArch.cards;
        double archValue = getCardsArchValueForDeckArch(nonlandCards, archCards);
        if (archValue == 0.0){
            continue;
        }
        if (archValue > deckArchFirstValue) {
            deckArchThird = deckArchSecondary;
            deckArchThirdValue = deckArchSecondaryValue;
            deckArchSecondary = deckArchFirst;
            deckArchSecondaryValue = deckArchFirstValue;
            deckArchFirst = deckArch;
            deckArchFirstValue = archValue;
        }
        if (archValue > deckArchSecondaryValue && deckArch.id != deckArchFirst.id) {
            deckArchThird = deckArchSecondary;
            deckArchThirdValue = deckArchSecondaryValue;
            deckArchSecondary = deckArch;
            deckArchSecondaryValue = archValue;
        }
        if (archValue > deckArchThirdValue && deckArch.id != deckArchFirst.id &&
                deckArch.id != deckArchSecondary.id) {
            deckArchThird = deckArch;
            deckArchThirdValue = archValue;
        }
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1: %2\n").arg(deckArch.name).arg(archValue));
        }
    }
    // Check the difference first and secondary most likely arch
    double deckArchsDiff = deckArchFirstValue - deckArchSecondaryValue;
    double deckArchsDiffPercent = deckArchsDiff / deckArchFirstValue * 100;
    if (LOG_DECK_ARCH_CALC) {
        LOGD(QString("%1:%2 \n%3:%4\n Diff %5 : %6").arg(deckArchFirst.name)
             .arg(deckArchFirstValue).arg(deckArchSecondary.name).arg(deckArchSecondaryValue)
             .arg(deckArchsDiff).arg(deckArchsDiffPercent));
    }
    if (deckArchsDiffPercent < 10.0) {
        // Calculate similarity value with lands
        double archFirstLandsValue = getCardsArchValueForDeckArch(cards, deckArchFirst.cards);
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1: %2 (With lands)").arg(deckArchFirst.name).arg(archFirstLandsValue));
        }
        QMap<int, int> archSecondaryCards = deckArchSecondary.cards;
        double archSecondaryLandsValue = getCardsArchValueForDeckArch(cards, archSecondaryCards);
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1: %2 (With lands)\n").arg(deckArchSecondary.name)
                 .arg(archSecondaryLandsValue));
        }
        DeckArch arch = archSecondaryLandsValue > archFirstLandsValue
                ? deckArchSecondary : deckArchFirst;
        double archLandsValue = archSecondaryLandsValue > archFirstLandsValue
                ? archSecondaryLandsValue : archFirstLandsValue;
        // Check the difference first and third most likely arch
        double deckArchsDiff = deckArchFirstValue - deckArchThirdValue;
        double deckArchsDiffPercent = deckArchsDiff / deckArchFirstValue * 100;
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("%1:%2 \n%3:%4\n Diff %5 : %6").arg(deckArchFirst.name)
                 .arg(deckArchFirstValue).arg(deckArchThird.name).arg(deckArchThirdValue)
                 .arg(deckArchsDiff).arg(deckArchsDiffPercent));
        }
        if (deckArchsDiffPercent < 10.0) {
            double archThirdLandsValue = getCardsArchValueForDeckArch(cards, deckArchThird.cards);
            if (LOG_DECK_ARCH_CALC) {
                LOGD(QString("--- %1: %2 (With lands)").arg(deckArchThird.name).arg(archThirdLandsValue));
            }
            return archThirdLandsValue > archLandsValue ? deckArchThird.name : arch.name;
        }
        return arch.name;
    } else {
        return deckArchFirst.name;
    }
}

double MtgDecksArch::getCardsArchValueForDeckArch(QMap<Card*, int> cards, QMap<int, int> archCards)
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
