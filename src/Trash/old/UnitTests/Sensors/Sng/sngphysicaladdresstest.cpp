#include "sngphysicaladdresstest.h"
#include <QDebug>

void SngPhysicalAddressTest::constructorTest()
{
    QFETCH(QString, src);
    QFETCH(char, expRes1);
    QFETCH(char, expRes2);
    QFETCH(char, expRes3);

    SngPhysicalAddress addr(src);

    QCOMPARE(addr.getByteAt(0), expRes1);
    QCOMPARE(addr.getByteAt(1), expRes2);
    QCOMPARE(addr.getByteAt(2), expRes3);

}

void SngPhysicalAddressTest::constructorTest_data()
{
    QTest::addColumn<QString>("src");
    QTest::addColumn<char>("expRes1");
    QTest::addColumn<char>("expRes2");
    QTest::addColumn<char>("expRes3");
    QTest::newRow("0") << "0.0.0" << '\x00' << '\x00' << '\x00';
    QTest::newRow("1") << "1.2.3" << '\x01' << '\x02' << '\x03';
    QTest::newRow("2") << "8.42.12" << '\x08' << '\x2a' << '\x0c';
    QTest::newRow("3") << "12.0.127" << '\x0c' << '\x00' << '\x7f';
}
