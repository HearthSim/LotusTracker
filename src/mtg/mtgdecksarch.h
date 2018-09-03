#ifndef MTGDECKSARCH_H
#define MTGDECKSARCH_H

#include "../entity/card.h"

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
    double getCardsArchValueForDeckArch(QMap<Card*, int> cards, QMap<int, double> archCards);

    QString dataDir;
    QNetworkAccessManager networkManager;
    QMap<QString, QMap<int, double>> deckArchs;

public:
    MtgDecksArch(QObject *parent = nullptr);
    QString findDeckArchitecture(QMap<Card*, int> cards);
    void updateDecksArchitectureFromAPI();

signals:

public slots:
};

#endif // MTGDECKSARCH_H
