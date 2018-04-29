#ifndef MTGALOGPARSER_H
#define MTGALOGPARSER_H

#include "entities.h"

#include <QObject>

class MtgaLogParser : public QObject
{
    Q_OBJECT

private:
    QList<int> msgResponseNumbers;
    void parseMsg(QPair<QString, QString> msg);
    void parsePlayerInventory(QString json);
    void parsePlayerInventoryUpdate(QString json);
    void parsePlayerCollection(QString json);
    void parsePlayerDecks(QString json);
    void parseOpponentInfo(QString json);
    void parseMatchInfo(QString json);
    void parsePlayerRankInfo(QString json);
    void parsePlayerDeckSelected(QString json);
    void parsePlayerMulliganInfo(QString json);
    void parsePlayerMatchState(QString json);

public:
    explicit MtgaLogParser(QObject *parent = nullptr);
    void parse(QString logNewContent);

signals:

public slots:
};

#endif // MTGALOGPARSER_H
