#include "../src/mtg/mtgalogparser.h"
#include "macros_test.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIODevice>

#include <QSignalSpy>
#include <QtTest/QtTest>

Q_DECLARE_METATYPE(PlayerInventory)

class TestMtgaLogParser: public QObject
{
    Q_OBJECT
private:
    MtgaLogParser *mtgaLogParser;

public:
    TestMtgaLogParser()
    {
        mtgaLogParser = new MtgaLogParser(this);
    }

private slots:
    void testParsePlayerInventory()
	{
        qRegisterMetaType<PlayerInventory>("PlayerInventory");
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

};
