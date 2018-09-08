#ifndef MTGDECKSARCH_H
#define MTGDECKSARCH_H

#include "../entity/card.h"
#include "../entity/deckarch.h"

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class MtgDecksArch : public QObject
{
    Q_OBJECT

private:
    void downloadDecksArch();
    void downloadDecksArchOnFinish();
    void loadDecksArch();
    void loadDecksArchFromFile();
    double getCardsSimilarityForDeckArch(QMap<Card*, int> cards, QMap<int, double> archCards);
    double getSimilarityPercentDifference(QPair<DeckArch, double> archFirst,
                                         QPair<DeckArch, double> archSecond);
    static bool deckArchSimilarityComparator(const QPair<DeckArch, double>& first,
                                             const QPair<DeckArch, double>& second);

    QString dataDir;
    QNetworkAccessManager networkManager;
    QList<DeckArch> deckArchs;

public:
    MtgDecksArch(QObject *parent = nullptr);
    QString findDeckArchitecture(QMap<Card*, int> cards);
    void updateDecksArchitectureFromAPI();

signals:

public slots:
};

#endif // MTGDECKSARCH_H
