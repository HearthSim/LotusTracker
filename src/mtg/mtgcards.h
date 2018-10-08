#ifndef MTGCARDS_H
#define MTGCARDS_H

#include "../entity/card.h"

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QRegExp>

class MtgCards : public QObject
{
    Q_OBJECT

private:
    void updateMtgaSetsFromAPIRequestOnFinish();
    void downloadSet(QString setCodeVersion);
    void downloadSetOnFinish();
    void loadSet(QString setCode);
    void loadSetFromFile(QString setFileName);
    Card* jsonObject2Card(QJsonObject jsonCard, QString setCode);
    QList<QChar> getBoderColorUsingManaSymbols(QList<QString> manaSymbols, bool isArtifact);
    QList<QChar> getLandBorderColorUsingColorIdentity(QJsonObject jsonCard);
    Card* createSplitCard(Card* leftSide, Card* rightSide);

    QString setsDir;
    QMap<int, Card*> cards;		//indexed by mtgaId
    QNetworkAccessManager networkManager;

public:
    MtgCards(QObject *parent = nullptr);
    Card* findCard(int mtgaId);
    void getMtgaSetsFromAPI();
    void updateMtgaSetsFromAPI();

signals:

public slots:
};

#endif // MTGCARDS_H
