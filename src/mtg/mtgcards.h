#ifndef MTGCARDS_H
#define MTGCARDS_H

#include "../entity/card.h"

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class MtgCards : public QObject
{
    Q_OBJECT

private:
    void updateMtgaIdsFromAPIRequestOnFinish();
    void downloadSet(QString setCode);
    void downloadSetOnFinish();
    void loadSet(QString setCode);
    void loadSetFromFile(QString setFileName);
    Card* jsonObject2Card(QJsonObject jsonCard, QString setCode);
    QList<QChar> getBoderColorUsingManaCost(QString manaCost, bool isArtifact);
    QList<QChar> getBorderColorUsingColorIdentity(QJsonObject jsonCard, bool isArtifact);
    Card* createSplitCard(Card* leftSide, Card* rightSide);

    QString setsDir;
    QMap<QString, QMap<QString, int>> mtgaIds;
    QMap<int, Card*> cards;		//indexed by mtgaId
    QNetworkAccessManager networkManager;

public:
    MtgCards(QObject *parent = nullptr);
    Card* findCard(int mtgaId);
    void updateMtgaIdsFromAPI();

signals:

public slots:
};

#endif // MTGCARDS_H
