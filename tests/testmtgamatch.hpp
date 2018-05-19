#include "../src/mtg/mtgalogparser.h"
#include "../src/mtg/mtgamatch.h"
#include "../src/macros.h"
#include "macros_test.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIODevice>

#include <QSignalSpy>
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(Card*)

class TestMtgaMatch: public QObject
{
    Q_OBJECT
private:
    MtgCards *mtgCards;
    MtgaLogParser *mtgaLogParser;
    MtgaMatch *mtgaMatch;
    QString readFile(QString fileName)
    {
        QFile msgFile(QString(":/res/%1").arg(fileName));
        if (!msgFile.open(QIODevice::ReadOnly | QFile::Text)) {
            LOGW(msgFile.errorString())
            return "";
        }
        return QTextStream(&msgFile).readAll();
    }

public:
    TestMtgaMatch()
    {
        mtgCards = new MtgCards(this);
        mtgaMatch = new MtgaMatch(this, mtgCards);
        mtgaLogParser = new MtgaLogParser(this, mtgCards);
        connect(mtgaLogParser, &MtgaLogParser::sgnMatchCreated,
                this, [this](MatchInfo matchInfo){
                    mtgaMatch->startNewMatch(matchInfo);
                });
        connect(mtgaLogParser, &MtgaLogParser::sgnMatchInfoSeats,
                mtgaMatch, &MtgaMatch::onMatchInfoSeats);
        connect(mtgaLogParser, &MtgaLogParser::sgnMatchInfoResultMatch,
                mtgaMatch, &MtgaMatch::onMatchInfoResultMatch);
        connect(mtgaLogParser, &MtgaLogParser::sgnSeatIdThatGoFirst,
                mtgaMatch, &MtgaMatch::onSeatIdThatGoFirst);
        connect(mtgaLogParser, &MtgaLogParser::sgnMatchStartZones,
                mtgaMatch, &MtgaMatch::onMatchStartZones);
        connect(mtgaLogParser, &MtgaLogParser::sgnMatchStateDiff,
                mtgaMatch, &MtgaMatch::onMatchStateDiff);
        connect(mtgaLogParser, &MtgaLogParser::sgnNewTurnStarted,
                mtgaMatch, &MtgaMatch::onNewTurnStarted);
    }

private slots:
    void testParsePlayerInventory()
    {
        qRegisterMetaType<Card*>();
        mtgaLogParser->parse(readFile("MatchCreated.txt"));
        mtgaLogParser->parse(readFile("MatchInfoSeats.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff0.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff1.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff2.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff3.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff4.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff5.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff6.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff7.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff8.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff9.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff10.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff11.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff12.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff13.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff14.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff15.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff16.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff17.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff18.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff19.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff20.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff21.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff22.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff23.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff24.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff25.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff26.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff27.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff28.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff29.txt"));
        mtgaLogParser->parse(readFile("game/GameStateDiff30.txt"));
    }

};
