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
    QTest::newRow("0") << "0.0.0" << '\x00' << '\x00' << '\x00';
    QTest::newRow("1") << "1.2.3" << '\x01' << '\x02' << '\x03';
    QTest::newRow("2") << "8.42.12" << '\x08' << '\x2a' << '\x0c';
    QTest::newRow("3") << "12.0.127" << '\x0c' << '\x00' << '\x7f';
    QTest::newRow("4") << "255.255.255" << '\xff' << '\xff' << '\xff';
    QTest::newRow("5") << "123.99.1" << '\x7b' << '\x63' << '\x01';
    QTest::newRow("6") << "12.15.19" << '\x0c' << '\x0f' << '\x13';
}

void SngMsgCreatorTest::prepareMsgDest()
{
    QFETCH(QString, src);
    QFETCH(char, expRes1);
    QFETCH(char, expRes2);
    QFETCH(char, expRes3);
    char msg[14];

    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, SngPhysicalAddress(src), defaultType, defaultValue);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[5], expRes1);
    QCOMPARE(msg[6], expRes2);
    QCOMPARE(msg[7], expRes3);
}

void SngMsgCreatorTest::prepareMsgDest_data()
{
    QTest::addColumn<QString>("src");
    QTest::addColumn<char>("expRes1");
    QTest::addColumn<char>("expRes2");
    QTest::addColumn<char>("expRes3");
    QTest::newRow("0") << "0.0.0" << '\x00' << '\x00' << '\x00';
    QTest::newRow("1") << "1.2.3" << '\x01' << '\x02' << '\x03';
    QTest::newRow("2") << "8.42.12" << '\x08' << '\x2a' << '\x0c';
    QTest::newRow("3") << "12.0.127" << '\x0c' << '\x00' << '\x7f';
    QTest::newRow("4") << "255.255.255" << '\xff' << '\xff' << '\xff';
    QTest::newRow("5") << "123.99.1" << '\x7b' << '\x63' << '\x01';
    QTest::newRow("6") << "12.15.19" << '\x0c' << '\x0f' << '\x13';
    QTest::newRow("7") << "2.1.1" << '\x02' << '\x01' << '\x01';
    QTest::newRow("8") << "2.2.5" << '\x02' << '\x02' << '\x05';
}

void SngMsgCreatorTest::prepareMsgBeginEnd()
{
    char msg[14];
    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, defaultAddr, defaultType, defaultValue);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[0], '\xff');
    QCOMPARE(msg[1], '\xff');
    QCOMPARE(msg[13], '\xfe');
}

void SngMsgCreatorTest::prepareMsgOnOff()
{
    char msgOn[14];
    char msgOff[14];
    SngMsgCreator msgCreator;
    SngFrame frameOn(defaultAddr, defaultAddr, SngOnOff, "on");
    SngFrame frameOff(defaultAddr, defaultAddr, SngOnOff, "off");

    msgCreator.prepareMsg(frameOn, msgOn);
    msgCreator.prepareMsg(frameOff, msgOff);

    QCOMPARE(msgOn[8], '\x01');
    QCOMPARE(msgOn[9], '\x01');
    QCOMPARE(msgOn[10], '\x00');
    QCOMPARE(msgOn[11], '\x00');
    QCOMPARE(msgOn[12], '\x00');

    QCOMPARE(msgOff[8], '\x01');
    QCOMPARE(msgOff[9], '\x00');
    QCOMPARE(msgOff[10], '\x00');
    QCOMPARE(msgOff[11], '\x00');
    QCOMPARE(msgOff[12], '\x00');
}

void SngMsgCreatorTest::prepareMsgDimm()
{
    QFETCH(QString, valueType);
    QFETCH(char, expRes);
    char msg[14];
    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, defaultAddr, SngDimm, valueType);

    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[8], '\x02');
    QCOMPARE(msg[9], expRes);
    QCOMPARE(msg[10], '\x00');
    QCOMPARE(msg[11], '\x00');
    QCOMPARE(msg[12], '\x00');
}

void SngMsgCreatorTest::prepareMsgDimm_data()
{
    QTest::addColumn<QString>("valueType");
    QTest::addColumn<char>("expRes");
    QTest::newRow("0") << "inc" << '\x09';
    QTest::newRow("1") << "endInc" << '\x08';
    QTest::newRow("2") << "dec" << '\x01';
    QTest::newRow("3") << "endDec" << '\x00';
}

void SngMsgCreatorTest::prepareMsgTime()
{
    QFETCH(QString, time);
    QFETCH(char, hours);
    QFETCH(char, min);
    QFETCH(char, sec);
    char msg[14];

    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, defaultAddr, SngTime, time);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[8], '\x03');
    QCOMPARE(msg[9], hours);
    QCOMPARE(msg[10], min);
    QCOMPARE(msg[11], sec);
    QCOMPARE(msg[12], '\x00');
}

void SngMsgCreatorTest::prepareMsgTime_data()
{
    QTest::addColumn<QString>("time");
    QTest::addColumn<char>("hours");
    QTest::addColumn<char>("min");
    QTest::addColumn<char>("sec");
    QTest::newRow("0") << "12:48:31" << '\x0c' << '\x30' << '\x1f';
    QTest::newRow("1") << "1:02:03" << '\x01' << '\x02' << '\x03';
    QTest::newRow("2") << "8:42:12" << '\x08' << '\x2a' << '\x0c';
    QTest::newRow("3") << "12:00:00" << '\x0c' << '\x00' << '\x00';
    QTest::newRow("4") << "0:00:00" << '\x00' << '\x00' << '\x00';
    QTest::newRow("5") << "23:59:59" << '\x17' << '\x3b' << '\x3b';
    QTest::newRow("6") << "12:15:19" << '\x0c' << '\x0f' << '\x13';
}

void SngMsgCreatorTest::prepareMsgDate()
{
    QFETCH(QString, date);
    QFETCH(char, day);
    QFETCH(char, month);
    QFETCH(char, year);
    char msg[14];

    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, defaultAddr, SngDate, date);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[8], '\x04');
    QCOMPARE(msg[9], day);
    QCOMPARE(msg[10], month);
    QCOMPARE(msg[11], year);
    QCOMPARE(msg[12], '\x00');
}

void SngMsgCreatorTest::prepareMsgDate_data()
{
    QTest::addColumn<QString>("date");
    QTest::addColumn<char>("day");
    QTest::addColumn<char>("month");
    QTest::addColumn<char>("year");
    QTest::newRow("0") << "15.07.1410" << '\x0f' << '\x07' << '\x87';
    QTest::newRow("1") << "1.02.1993" << '\x01' << '\x02' << '\xd0';
    QTest::newRow("2") << "19.07.1990" << '\x13' << '\x07' << '\xcd';
    QTest::newRow("3") << "15.01.2012" << '\x0f' << '\x01' << '\xe3';
    QTest::newRow("4") << "29.02.2000" << '\x1d' << '\x02' << '\xd7';
    QTest::newRow("5") << "31.12.1999" << '\x1f' << '\x0c' << '\xd6';
    QTest::newRow("6") << "9.09.1909" << '\x09' << '\x09' << '\x7c';
}

void SngMsgCreatorTest::prepareMsgTemp()
{
    QFETCH(QString, temp);
    QFETCH(char, integ);
    QFETCH(char, frac);
    QFETCH(char, minus);
    char msg[14];

    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, defaultAddr, SngTemp, temp);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[8], '\x05');
    QCOMPARE(msg[9], integ);
    QCOMPARE(msg[10], frac);
    QCOMPARE(msg[11], minus);
    QCOMPARE(msg[12], '\x00');
}

void SngMsgCreatorTest::prepareMsgTemp_data()
{
    QTest::addColumn<QString>("temp");
    QTest::addColumn<char>("integ");
    QTest::addColumn<char>("frac");
    QTest::addColumn<char>("minus");
    QTest::newRow("0") << "-21.5" << '\x15' << '\x05' << '\x01';
    QTest::newRow("1") << "36.6" << '\x24' << '\x06' << '\x00';
    QTest::newRow("2") << "42.0" << '\x2a' << '\x00' << '\x00';
    QTest::newRow("3") << "-10.23" << '\x0a' << '\x17' << '\x01';
    QTest::newRow("4") << "-4.0" << '\x04' << '\x00' << '\x01';
    QTest::newRow("5") << "35.9" << '\x23' << '\x09' << '\x00';
    QTest::newRow("6") << "25.45" << '\x19' << '\x2d' << '\x00';
}

void SngMsgCreatorTest::prepareMsgValue()
{
    QFETCH(QString, value);
    QFETCH(char, expRes);
    char msg[14];

    SngMsgCreator msgCreator;
    SngFrame frame(defaultAddr, defaultAddr, SngValue, value);
    msgCreator.prepareMsg(frame, msg);

    QCOMPARE(msg[8], '\x06');
    QCOMPARE(msg[9], expRes);
    QCOMPARE(msg[10], '\x00');
    QCOMPARE(msg[11], '\x00');
    QCOMPARE(msg[12], '\x00');
}

void SngMsgCreatorTest::prepareMsgValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<char>("expRes");
    QTest::newRow("0") << "0" << '\x00';
    QTest::newRow("1") << "36" << '\x24';
    QTest::newRow("2") << "42" << '\x2a';
    QTest::newRow("3") << "127" << '\x7f';
    QTest::newRow("4") << "99" << '\x63';
    QTest::newRow("5") << "255" << '\xff';
    QTest::newRow("6") << "203" << '\xcb';
}

