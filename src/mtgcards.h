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
    QString setsDir;
	QMap<QString, QList<Card>> sets;
	QNetworkAccessManager networkManager;

public:
    MtgCards(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MTGCARDS_H
