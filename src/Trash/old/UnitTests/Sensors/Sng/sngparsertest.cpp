#include "sngparsertest.h"
#include <QDebug>

void SngMsgParserTest::init(char * msg)
{
    for (int i = 2; i < 13; i++)
        msg[i] = 0;
    msg[0] = msg[1] = '\xff';
    msg[13] = '\xfe';
    msg[8] = '\x01';
}

void SngMsgParserTest::parseMsgSrc()
{
    QFETCH(QString, expSrc);
    QFETCH(char, byte1);
    QFETCH(char, byte2);
    QFETCH(char, byte3);

    char msg[14];
    init(msg);
    msg[2] = byte1;
    msg[3] = byte2;
    msg[4] = byte3;

    SngMsgParser msgParser;
    SngFrame frame;

    QCOMPARE(msgParser.parseMsg(msg, frame), false);
    QCOMPARE(frame.src.toString(), expSrc);
}

void SngMsgParserTest::parseMsgSrc_data()
{
    QTest::addColumn<QString>("expSrc");
    QTest::addColumn<char>("byte1");
    QTest::addColumn<char>("byte2");
    QTest::addColumn<char>("byte3");
    QTest::newRow("0") << "0.0.0" << '\x00' << '\x00' << '\x00';
    QTest::newRow("1") << "1.2.3" << '\x01' << '\x02' << '\x03';
    QTest::newRow("2") << "8.42.12" << '\x08' << '\x2a' << '\x0c';
    QTest::newRow("3") << "12.0.127" << '\x0c' << '\x00' << '\x7f';
    QTest::newRow("4") << "255.255.255" << '\xff' << '\xff' << '\xff';
    QTest::newRow("5") << "123.99.1" << '\x7b' << '\x63' << '\x01';
    QTest::newRow("6") << "12.15.19" << '\x0c' << '\x0f' << '\x13';
}

void SngMsgParserTest::parseMsgDest()
{
    QFETCH(QString, expDest);
    QFETCH(char, byte1);
    QFETCH(char, byte2);
    QFETCH(char, byte3);

    char msg[14];
    init(msg);
    msg[5] = byte1;
    msg[6] = byte2;
    msg[7] = byte3;

    SngMsgParser msgParser;
    SngFrame frame;

    QCOMPARE(msgParser.parseMsg(msg, frame), false);
    QCOMPARE(frame.dest.toString(), expDest);
}

void SngMsgParserTest::parseMsgDest_data()
{
    QTest::addColumn<QString>("expDest");
    QTest::addColumn<char>("byte1");
    QTest::addColumn<char>("byte2");
    QTest::addColumn<char>("byte3");
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

void SngMsgParserTest::parseMsgBeginEnd()
{
    char msg[14];

    SngMsgParser msgParser;
    SngFrame frame;

    QCOMPARE(msgParser.parseMsg(msg, frame), true);

    init(msg);
    QCOMPARE(msgParser.parseMsg(msg, frame), false);

    init(msg);
    msg[0] = '\x42';
    QCOMPARE(msgParser.parseMsg(msg, frame), true);

    init(msg);
    msg[1] = '\xfe';
    QCOMPARE(msgParser.parseMsg(msg, frame), true);

    init(msg);
    msg[13] = '\xfd';
    QCOMPARE(msgParser.parseMsg(msg, frame), true);
}

void SngMsgParserTest::parseMsgOnOff()
{
    char msgOn[14];
    char msgOff[14];
    init(msgOn);
    init(msgOff);
    msgOn[9] = '\x01';

    SngMsgParser msgParser;
    SngFrame frameOn, frameOff;

    QCOMPARE(msgParser.parseMsg(msgOn, frameOn), false);
    QCOMPARE(frameOn.type, SngOnOff);
    QCOMPARE(frameOn.value, QString("on"));

    QCOMPARE(msgParser.parseMsg(msgOff, frameOff), false);
    QCOMPARE(frameOff.type, SngOnOff);
    QCOMPARE(frameOff.value, QString("off"));
}

void SngMsgParserTest::parseMsgDimm()
{
    QFETCH(QString, valueType);
    QFETCH(char, valueByte);
    char msg[14];
    init(msg);

    SngMsgParser msgParser;
    SngFrame frame;

    msg[8] = '\x02';
    msg[9] = valueByte;

    QCOMPARE(msgParser.parseMsg(msg, frame), false);
    QCOMPARE(frame.type, SngDimm);
    QCOMPARE(frame.value, valueType);
}

void SngMsgParserTest::parseMsgDimm_data()
{
    QTest::addColumn<QString>("valueType");
    QTest::addColumn<char>("valueByte");
    QTest::newRow("0") << "inc" << '\x09';
    QTest::newRow("1") << "endInc" << '\x08';
    QTest::newRow("2") << "dec" << '\x01';
    QTest::newRow("3") << "endDec" << '\x00';
}

void SngMsgParserTest::parseMsgTime()
{
    QFETCH(QString, time);
    QFETCH(char, hours);
    QFETCH(char, min);
    QFETCH(char, sec);

    char msg[14];
    init(msg);

    SngMsgParser msgParser;
    SngFrame frame;

    msg[8] = '\x03';
    msg[9] = hours;
    msg[10] = min;
    msg[11] = sec;

    QCOMPARE(msgParser.parseMsg(msg, frame), false);
    QCOMPARE(frame.type, SngTime);
//    qDebug() << frame.value << "|" << time;
    QCOMPARE(frame.value, time);
}

void SngMsgParserTest::parseMsgTime_data()
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

void SngMsgParserTest::parseMsgDate()
{
//    QFETCH(QString, date);
//    QFETCH(char, day);
//    QFETCH(char, month);
//    QFETCH(char, year);
//    char msg[14];

//    SngMsgCreator msgCreator;
//    SngFrame frame(defaultAddr, defaultAddr, SngDate, date);
//    msgCreator.prepareMsg(frame, msg);

//    QCOMPARE(msg[8], '\x04');
//    QCOMPARE(msg[9], day);
//    QCOMPARE(msg[10], month);
//    QCOMPARE(msg[11], year);
//    QCOMPARE(msg[12], '\x00');
}

void SngMsgParserTest::parseMsgDate_data()
{
//    QTest::addColumn<QString>("date");
//    QTest::addColumn<char>("day");
//    QTest::addColumn<char>("month");
//    QTest::addColumn<char>("year");
//    QTest::newRow("0") << "15.07.1410" << '\x0f' << '\x07' << '\x87';
//    QTest::newRow("1") << "1.02.1993" << '\x01' << '\x02' << '\xd0';
//    QTest::newRow("2") << "19.07.1990" << '\x13' << '\x07' << '\xcd';
//    QTest::newRow("3") << "15.01.2012" << '\x0f' << '\x01' << '\xe3';
//    QTest::newRow("4") << "29.02.2000" << '\x1d' << '\x02' << '\xd7';
//    QTest::newRow("5") << "31.12.1999" << '\x1f' << '\x0c' << '\xd6';
//    QTest::newRow("6") << "9.09.1909" << '\x09' << '\x09' << '\x7c';
}

void SngMsgParserTest::parseMsgTemp()
{
    QFETCH(QString, temp);
    QFETCH(char, integ);
    QFETCH(char, frac);
    QFETCH(char, minus);

    char msg[14];
    init(msg);

    SngMsgParser msgParser;
    SngFrame frame;

    msg[8] = '\x05';
    msg[9] = integ;
    msg[10] = frac;
    msg[11] = minus;

    QCOMPARE(msgParser.parseMsg(msg, frame), false);
    QCOMPARE(frame.type, SngTemp);
    QCOMPARE(frame.value, temp);
}

void SngMsgParserTest::parseMsgTemp_data()
{
    QTest::addColumn<QString>("temp");
    QTest::addColumn<char>("integ");
    QTest::addColumn<char>("frac");
    QTest::addColumn<char>("minus");
    QTest::newRow("0") << "-21.5" << '\x15' << '\x05' << '\x01';
    QTest::newRow("1") << "36.6" << '\x24' << '\x06' << '\x00';
    QTest::newRow("2") << "42.0" << '\x2a' << '\x00' << '\x00';
    QTest::newRow("3") << "-10.2" << '\x0a' << '\x02' << '\x01';
    QTest::newRow("4") << "-4.0" << '\x04' << '\x00' << '\x01';
    QTest::newRow("5") << "35.9" << '\x23' << '\x09' << '\x00';
    QTest::newRow("6") << "25.4" << '\x19' << '\x04' << '\x00';
}

void SngMsgParserTest::parseMsgValue()
{
    QFETCH(QString, expValue);
    QFETCH(char, valueByte);

    char msg[14];
    init(msg);

    SngMsgParser msgParser;
    SngFrame frame;

    msg[8] = '\x06';
    msg[9] = valueByte;

    QCOMPARE(msgParser.parseMsg(msg, frame), false);
    QCOMPARE(frame.type, SngValue);
    QCOMPARE(frame.value, expValue);
}

void SngMsgParserTest::parseMsgValue_data()
{
    QTest::addColumn<QString>("expValue");
    QTest::addColumn<char>("valueByte");
    QTest::newRow("0") << "0" << '\x00';
    QTest::newRow("1") << "36" << '\x24';
    QTest::newRow("2") << "42" << '\x2a';
    QTest::newRow("3") << "127" << '\x7f';
    QTest::newRow("4") << "99" << '\x63';
    QTest::newRow("5") << "255" << '\xff';
    QTest::newRow("6") << "203" << '\xcb';
}


