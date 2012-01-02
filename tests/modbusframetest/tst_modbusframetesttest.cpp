#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QByteArray>
#include <string>
#include <QDebug>
#include "/home/toolchains/telemetrics/src/Sensors/Modbus/modbusrtuframe.h"

class ModbusframetestTest : public QObject
{
    Q_OBJECT

public:
    ModbusframetestTest();

private Q_SLOTS:
    void testCase1();
    void testCase1_data();

private:
       ModbusRtuFrame* frame;
};

ModbusframetestTest::ModbusframetestTest(){
}

void ModbusframetestTest::testCase1()
{
    QFETCH(unsigned char, function);
    QFETCH(unsigned char, addr);
    QFETCH(short, data_length);
    QFETCH(unsigned short int, crc);
    frame = new ModbusRtuFrame(function, data_length);
    frame->setAddr(addr);
    //qDebug() << frame->getCRC();
    std::string s("");
    //qDebug() << data_length;
    for (int i = 0; i < data_length; i++)
        s += "a";
    frame->setData((unsigned char*) s.c_str(), data_length);
    QCOMPARE(frame->getFunction(), function);
    QCOMPARE(frame->getAddr(), addr);
    QCOMPARE(frame->showSize(), data_length);

    unsigned char** to_send = frame->toSend();
    //qDebug() << to_send[0][0];
    QCOMPARE(to_send[0][0], addr);
    QCOMPARE(to_send[1][0], function);
    QCOMPARE((char*) to_send[2], s.c_str());

    QCOMPARE(frame->getCRC(), crc);


    frame->destroyFrame();
    delete frame;
    s.clear();
}

void ModbusframetestTest::testCase1_data()
{
    QTest::addColumn<unsigned char>("function");
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<short>("data_length");
    QTest::addColumn<unsigned short int>("crc");
    const int tests_Number = 10;
    short data_lengths[tests_Number] = {12,23,13,14,15,4,65,78,29,11};
    unsigned char** tmp = new unsigned char*[tests_Number];
    for (int i = 0; i < tests_Number; i++) {
        tmp[i] = new unsigned char[data_lengths[i] + 2];
        tmp[i][0] = (unsigned char) '1';
        tmp[i][1] = (unsigned char) '4';
        for (int j = 0; j < data_lengths[i]; j++) {
            tmp[i][j+2] = 'a'; // fake data
        }
    }
    for (int i = 0; i < tests_Number; i++) {
        QTest::newRow("0") << (unsigned char) '1' << (unsigned char) '4'
                           << (short) data_lengths[i] << qChecksum((const char*) tmp[i], data_lengths[i] + 2);
    }
    for (int i = 0; i < tests_Number; i++) {
        delete []tmp[i];
    }
    qDebug() << "poszlo";
    delete []tmp;
}

QTEST_APPLESS_MAIN(ModbusframetestTest);

#include "tst_modbusframetesttest.moc"
