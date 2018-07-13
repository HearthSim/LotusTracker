#ifndef MTGAIDS_H
#define MTGAIDS_H

#include <QDebug>
#include <QtGlobal>
#include <QJsonObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

#define LOG(msg) qDebug("%s", qUtf8Printable(msg))

class MtgaIDs : public QObject
{
    Q_OBJECT

private:
    void downloadSet(QString setCode);
    void downloadSetOnFinish();
    void loadSet(QString setCode, int firstCardId, QMap<QString, int> unorderedCardsId);
    void loadSetFromFile(QString setFileName);
    QMap<QString, int> getKLDUnorderedCardsId();
    QMap<QString, int> getAERUnorderedCardsId();
    QMap<QString, int> getW17UnorderedCardsId();
    QMap<QString, int> getAKHUnorderedCardsId();
    QMap<QString, int> getHOUUnorderedCardsId();
    QMap<QString, int> getXLNUnorderedCardsId();
    QMap<QString, int> getDOMUnorderedCardsId();

    QString setsDir;
    QMap<QString, int> setsFirstCardId;
    QMap<QString, QMap<QString, int>> setsUnorderedCardIds;
	QNetworkAccessManager networkManager;

public:
    MtgaIDs(QObject *parent = nullptr);
    void process();

signals:

public slots:
};

#endif // MTGAIDS_H
