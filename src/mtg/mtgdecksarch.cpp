#include "mtgdecksarch.h"
#include "../urls.h"
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
#define DIFF_SIMILARITY_THRESHOLD 10.0

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
        QMap<int, double> cards;
        for (QString archCard : archCards.keys()) {
            cards[archCard.toInt()] = archCards[archCard].toDouble();
        }
        deckArchs << DeckArch(archId, archName, archColors, cards);
    }

    LOGI(QString("Decks architectures loaded with %1 archtectures")
         .arg(jsonDecksArchitecture.count()));
}

void MtgDecksArch::downloadDecksArch()
{
    QUrl url(QString("%1/decksarch").arg(URLs::API()));
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
    QString deckColors = Deck::calcColorIdentity(cards, true);
    QMap<Card*, int> nonlandCards;
    for (Card* card : cards.keys()) {
        if (!card->isLand) {
            nonlandCards[card] = cards[card];
        }
    }
    // Archs with deck colors by similarity
    QList<QPair<DeckArch, double>> archsWithDeckColors;
    for (DeckArch deckArch : deckArchs) {
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1:").arg(deckArch.name));
        }
        QMap<int, double> archCards = deckArch.cards;
        bool archContainsColor = true;
        for (QChar deckColor : deckColors) {
            if (!deckArch.colors.contains(deckColor, Qt::CaseInsensitive)) {
                archContainsColor = false;
            }
        }
        if (!archContainsColor) {
            continue;
        }
        double archSimilarity = getCardsSimilarityForDeckArch(nonlandCards, archCards);
        if (archSimilarity == 0.0){
            continue;
        }
        archsWithDeckColors << qMakePair(deckArch, archSimilarity);
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- %1 added with %2\n").arg(deckArch.name).arg(archSimilarity));
        }
    }
    std::sort(archsWithDeckColors.begin(), archsWithDeckColors.end(), deckArchSimilarityComparator);
    if (archsWithDeckColors.isEmpty()) {
        return tr("Unknown");
    }
    if (archsWithDeckColors.size() == 1) {
        return archsWithDeckColors[0].first.name;
    }
    // Check the difference first and secondary most likely arch
    double diff = getSimilarityPercentDifference(archsWithDeckColors[0], archsWithDeckColors[1]);
    if (diff < DIFF_SIMILARITY_THRESHOLD) {
        DeckArch archFirst = archsWithDeckColors[0].first;
        DeckArch archSecond = archsWithDeckColors[1].first;
        // Calculate similarity value with lands
        double archFirstSimilarityWithLands = getCardsSimilarityForDeckArch(cards, archFirst.cards);
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- First Arch --- %1: %2 (With lands)")
                 .arg(archFirst.name).arg(archFirstSimilarityWithLands));
        }
        double archSecondSimilarityWithLands = getCardsSimilarityForDeckArch(cards, archSecond.cards);
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("--- Second Arch --- %1: %2 (With lands)\n")
                 .arg(archSecond.name).arg(archSecondSimilarityWithLands));
        }
        DeckArch arch = archSecondSimilarityWithLands > archFirstSimilarityWithLands
                ? archSecond : archFirst;
        if (archsWithDeckColors.size() == 2) {
            return arch.name;
        }
        double archSimilarityWithLands = archSecondSimilarityWithLands > archFirstSimilarityWithLands
                ? archSecondSimilarityWithLands : archFirstSimilarityWithLands;
        // Check the difference first and third most likely arch
        double diff2 = getSimilarityPercentDifference(archsWithDeckColors[0], archsWithDeckColors[2]);
        if (diff2 < DIFF_SIMILARITY_THRESHOLD) {
            DeckArch archThird = archsWithDeckColors[2].first;
            double archThirdSimilarityWithLands = getCardsSimilarityForDeckArch(cards, archThird.cards);
            if (LOG_DECK_ARCH_CALC) {
                LOGD(QString("--- Third Arch --- %1: %2 (With lands)")
                     .arg(archThird.name).arg(archThirdSimilarityWithLands));
            }
            return archThirdSimilarityWithLands > archSimilarityWithLands
                    ? archThird.name : arch.name;
        }
        return arch.name;
    } else {
        return archsWithDeckColors[0].first.name;
    }
}

double MtgDecksArch::getCardsSimilarityForDeckArch(QMap<Card*, int> cards, QMap<int, double> archCards)
{
    double archValue = 0.0;
    for (Card* card : cards.keys()) {
        int cardQtd = cards[card];
        double archCardQtdAverage = archCards[card->mtgaId];
        if (archCardQtdAverage == 0.0) {
            continue;
        }
        double cardQtdAveragePercentValue = (archCardQtdAverage < cardQtd) ?
                    archCardQtdAverage / cardQtd : cardQtd / archCardQtdAverage;
        double cardRelevance = card->isBasicLand() ? 1 : archCardQtdAverage / 4;
        double cardRelevanceValue = cardQtdAveragePercentValue * cardRelevance;
        double cardValue = cardRelevanceValue * archCardQtdAverage; //Penalize cards above average
        archValue += cardValue;
        if (LOG_DECK_ARCH_CALC) {
            LOGD(QString("%1: %2 of %3 = %4% * %5(Rel) * %6(Avg) = %7").arg(card->name).arg(cardQtd)
                 .arg(archCardQtdAverage).arg(cardQtdAveragePercentValue).arg(cardRelevance)
                 .arg(archCardQtdAverage).arg(cardValue));
        }
    }
    return archValue;
}

double MtgDecksArch::getSimilarityPercentDifference(QPair<DeckArch, double> archFirst,
                                                   QPair<DeckArch, double> archSecond)
{
    double deckArchsDiff = archFirst.second - archSecond.second;
    double deckArchsDiffPercent = deckArchsDiff / archFirst.second * 100;
    if (LOG_DECK_ARCH_CALC) {
        LOGD(QString("%1:%2 \n%3:%4\n Diff %5 : %6").arg(archFirst.first.name)
             .arg(archFirst.second).arg(archSecond.first.name).arg(archSecond.second)
             .arg(deckArchsDiff).arg(deckArchsDiffPercent));
    }
    return deckArchsDiffPercent;
}

bool MtgDecksArch::deckArchSimilarityComparator(const QPair<DeckArch, double>& first,
                                                const QPair<DeckArch, double>& second)
{
    return first.second > second.second;
}
