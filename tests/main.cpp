#include "testmtgalogparser.hpp"
#include "testmtgamatch.hpp"

#include <QtTest/QtTest>

int main(int argc, char *argv[])
{
	int status = 0;
	auto runTest = [&status, argc, argv](QObject* obj) {
        status |= QTest::qExec(obj, argc, argv);
    };

    runTest(new TestMtgaMatch);
    runTest(new TestMtgaLogParser);
    return status;
}
