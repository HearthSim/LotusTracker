#include "../src/mtg/mtgalogparser.h"
#include "macros_test.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIODevice>

#include <QSignalSpy>
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(Deck)
Q_DECLARE_METATYPE(Match)
Q_DECLARE_METATYPE(MatchPlayer)
Q_DECLARE_METATYPE(PlayerInventory)

class TestMtgaLogParser: public QObject
{
    Q_OBJECT
private:
    MtgCards *mtgCards;
    MtgaLogParser *mtgaLogParser;

public:
    TestMtgaLogParser()
    {
        mtgCards = new MtgCards(this);
        mtgaLogParser = new MtgaLogParser(this, mtgCards);
    }

private slots:
    void testParsePlayerInventory()
    {
        qRegisterMetaType<PlayerInventory>();
        QString log;
        READ_LOG("PlayerInventory.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnPlayerInventory);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        PlayerInventory playerInventory = args.first().value<PlayerInventory>();
        QVERIFY(playerInventory.wcMythic == 6);
    }

    void testParsePlayerInventoryUpdate()
    {
        QString log;
        READ_LOG("PlayerInventoryUpdate.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnPlayerInventoryUpdate);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QList<int> newCards = args.first().value<QList<int>>();
        QVERIFY(newCards.first() == 65963);
    }

    void testParsePlayerCollection()
    {
        qRegisterMetaType<QMap<int, int>>();
        QString log;
        READ_LOG("PlayerCollection.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnPlayerCollection);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QMap<int, int> playerCollection = args.first().value<QMap<int, int>>();
        QVERIFY(playerCollection.size() == 421);
        QVERIFY(playerCollection[66041] == 3);
    }

    void testParsePlayerDecks()
    {
        qRegisterMetaType<QList<Deck>>();
        QString log;
        READ_LOG("PlayerDecks.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnPlayerDecks);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QList<Deck> playerDecks = args.first().value<QList<Deck>>();
        QVERIFY(playerDecks.size() == 3);
        Card* vraskasContemptCard = mtgCards->findCard(66223);
        QVERIFY(playerDecks.first().cards[vraskasContemptCard] == 3);
    }

    void testParseMatchCreated()
    {
        qRegisterMetaType<Match>();
        QString log;
        READ_LOG("MatchCreated.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnMatchCreated);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        Match match = args.first().value<Match>();
        QVERIFY(match.opponentRankClass == "Beginner");
        QVERIFY(match.opponentRankTier == 1);
    }

    void testParseMatchInfoSeats()
    {
        qRegisterMetaType<QList<MatchPlayer>>();
        QString log;
        READ_LOG("MatchInfoSeats.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnMatchInfoSeats);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QList<MatchPlayer> matchPlayers = args.first().value<QList<MatchPlayer>>();
        MatchPlayer matchPlayer = matchPlayers.first();
        QVERIFY(matchPlayer.name == "Edipo2s" && matchPlayer.seat == 1);
    }

    void testParseMatchInfoMatchResult()
    {
        QString log;
        READ_LOG("MatchInfoResult.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnMatchInfoResultMatch);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        int matchWinningTeamId = args.first().toInt();
        QVERIFY(matchWinningTeamId == 1);
    }

    void testParsePlayerRankInfo()
    {
        qRegisterMetaType<QPair<QString, int>>();
        QString log;
        READ_LOG("PlayerRankInfo.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnPlayerRankInfo);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        QPair<QString, int> playerRankInfo = args.first().value<QPair<QString, int>>();
        QVERIFY(playerRankInfo.first == "Intermediate" && playerRankInfo.second == 1);
    }

    void testParsePlayerDeckSelected()
    {
        qRegisterMetaType<Deck>();
        QString log;
        READ_LOG("PlayerDeckSelected.txt", log);
        QSignalSpy spy(mtgaLogParser, &MtgaLogParser::sgnPlayerDeckSelected);
        mtgaLogParser->parse(log);

        QCOMPARE(spy.count(), 1);
        QList<QVariant> args = spy.takeFirst();
        Deck playerDeckSelected = args.first().value<Deck>();
        Card* forerunnerOfTheEmpireCard = mtgCards->findCard(66821);
        QVERIFY(playerDeckSelected.cards[forerunnerOfTheEmpireCard] == 2);
    }

};
