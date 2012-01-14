#include "sngmsgcreatortest.h"
#include <QDebug>

void SngMsgCreatorTest::prepareMsgSrc()
{
    QFETCH(QString, src);
    QFETCH(char, expRes1);
    QFETCH(char, expRes2);
    QFETCH(char, expRes3);
    char msg[14];

    SngMsgCreator msgCreator;
    SngFrame frame(SngPhysicalAddress(src), defaultAddr, defaultType, defaultValue);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[2], expRes1);
    QCOMPARE(msg[3], expRes2);
    QCOMPARE(msg[4], expRes3);
}

void SngMsgCreatorTest::prepareMsgSrc_data()
{
    QTest::addColumn<QString>("src");
    QTest::addColumn<char>("expRes1");
    QTest::addColumn<char>("expRes2");
    QTest::addColumn<char>("expRes3");
    QTest::newRow("0") << "0.0.0" << '\0' << '\0' << '\0';
    QTest::newRow("1") << "1.2.3" << '\1' << '\2' << '\3';
    QTest::newRow("2") << "8.42.12" << '\8' << '\42' << '\12';
    QTest::newRow("3") << "12.0.127" << '\12' << '\0' << '\127';
//    QTest::newRow("4") << "255.255.255" << "\255\255\255";
//    QTest::newRow("5") << "123.99.1" << "\123\99\1";
//    QTest::newRow("6") << "12.15.19" << "\12\15\19";
}
