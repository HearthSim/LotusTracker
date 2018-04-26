#ifndef MTGCARDS_H
#define MTGCARDS_H

#include "entities.h"

#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class MtgCards : public QObject
{
    Q_OBJECT

private:
    void downloadSet(QString setCode);
    void downloadSetOnFinish();
    void loadSet(QString setCode);
    void loadSetFromFile(QString setFileName);
    int getMtgaId(QString setCode, QString cardNumber);

    QString setsDir;
    QMap<QString, QMap<QString, int>> mtgaIds;
	QMap<int, Card*> cards;		//indexed by mtgaId
	QNetworkAccessManager networkManager;

public:
    MtgCards(QObject *parent = nullptr);
    Card* getCard(int mtgaId);

signals:

public slots:
};

#endif // MTGCARDS_H
