#include "modbustest.h"
#include "src/Sensors/Modbus/modbus.h"
#include "src/Configurator/configurator.h"
#include "src/Message/message.h"

#include <QVector>
#include <QDebug>
#include <termios.h>
#include <string>
#include <errno.h>
#include <QByteArray>

int readed;
unsigned short crc;

short ModbusTest::countCRC(unsigned char *array, int array_length){
    crc = qChecksum((char*) array, array_length);
}

void ModbusTest::testPort(){ // Check out those parameters!
    std::string port2("/dev/pts/3");
    if ((fd = open(port2.c_str(), O_RDWR | O_NOCTTY)) < 0) //wywalamy O_NONBLOCK, wtedy dziala
        qDebug() << "Open test port failure" << "Errno: " << errno;
    termios* port_param = new termios;
    port_param->c_cflag = B9600 | CS8;
    port_param->c_iflag = IGNPAR;
    port_param->c_oflag = 0;
    port_param->c_lflag = 0;
    port_param->c_cc[VMIN] = 1;
    port_param->c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, port_param) < 0)
        qDebug() << "Setting test port attr error";
    qDebug() << "Test port is ready!";
    delete[] port_param;
}

//Rozdzielamy testy write/read



void ModbusTest::f1_2_3_4_Snd()
{
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(QString, NSize);
    QFETCH(unsigned char, testval);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    Message* mesg = new Message(function, NSize);
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);

    unsigned char* answer = new unsigned char[2];
    unsigned char* data = new unsigned char[4];
    //unsigned short crc;
    if ((readed = read(fd, answer, 2)) < 0)
        qDebug() << "Read error!" << errno; // czytamy adres i funkcje
    if ((readed = read(fd, data, 4)) < 0)
        qDebug() << "Read error!" << errno;
    if ((readed = read(fd, &crc, sizeof(short))) < 0)
        qDebug() << "CRC read error";
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[0]);
    QCOMPARE(data[3], (unsigned char) NSize.at(3).toAscii());
    delete[] answer;
    delete[] data;

    /*
    unsigned char* response = new unsigned char[5];
    response[0] = addr;
    response[1] = 0x01;
    response[2] = 2;
    response[3] = testval;
    response[4] = testval;
    if ((readed = write(fd, response, 5)) < 0)
        qDebug() << "Test: Write error!";
    qDebug() << "Writed: " << readed;
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
    qDebug() << returned[0].value; */
}

void ModbusTest::f1_2_3_4_Snd_data()
{
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<QString>("NSize");
    QTest::addColumn<unsigned char>("testval");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x01" << "\x52\x4B\x11\x11\x34\x67" << (unsigned char) 0xBB;
    QTest::newRow("1") << (unsigned char) 0x02 << "\x02" << "1342432" << (unsigned char) 0x3F;
    QTest::newRow("2") << (unsigned char) 0x03 << "\x03" << "324352" << (unsigned char) 0xCC;
    QTest::newRow("3") << (unsigned char) 0x02 << "\x01" << "125243" << (unsigned char) 0x45;
    QTest::newRow("4") << (unsigned char) 0x01 << "\x01" << "4234231" << (unsigned char) 0x52;
}


void ModbusTest::errorRec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, error);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    unsigned char* response = new unsigned char[3];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = error;
    crc = qChecksum((char*) response, 3);
    if ((readed = write(fd, response, 3)) < 0)
        qDebug() << "Test: Write error!";
    if ((readed = write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
}

void ModbusTest::errorRec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned char>("error");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x81" << (unsigned char) 0x01;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x82" << (unsigned char) 0x02;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x83" << (unsigned char) 0x03;
    QTest::newRow("3") << (unsigned char) 0x01 << "\x84" << (unsigned char) 0x02;
    QTest::newRow("4") << (unsigned char) 0x01 << "\x86" << (unsigned char) 0x04;
    QTest::newRow("5") << (unsigned char) 0x01 << "\x87" << (unsigned char) 0x03;
    QTest::newRow("6") << (unsigned char) 0x01 << "\x88" << (unsigned char) 0x02;
    QTest::newRow("7") << (unsigned char) 0x01 << "\x89" << (unsigned char) 0x03;
    QTest::newRow("8") << (unsigned char) 0x01 << "\x90" << (unsigned char) 0x04;
    QTest::newRow("9") << (unsigned char) 0x01 << "\x91" << (unsigned char) 0x03;
    QTest::newRow("10") << (unsigned char) 0x01 << "\xA0" << (unsigned char) 0x02;
    QTest::newRow("11") << (unsigned char) 0x01 << "\xA1" << (unsigned char) 0x03;
}

void ModbusTest::f1_2_3_4_Rec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, byte_count);
    QFETCH(unsigned char, data1);
    QFETCH(unsigned char, data2);
    QFETCH(unsigned char, data3);
    QFETCH(unsigned char, data4);
    QFETCH(unsigned char, data5);
    QFETCH(unsigned char, data6);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    unsigned char* response = new unsigned char[9];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = byte_count;
    response[3] = data1;
    response[4] = data2;
    response[5] = data3;
    response[6] = data4;
    response[7] = data5;
    response[8] = data6;
    QString tmp;
    tmp.append(data1).append(data2).append(data3).append(data4).append(data5).append(data6).append('\0');
    crc = qChecksum((char*) response, 9);
    if ((readed = write(fd, response, 9)) < 0)
        qDebug() << "Test: Write error!";
    if ((readed = write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
    QCOMPARE(tmp, returned[0].value);
}



void ModbusTest::f1_2_3_4_Rec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned char>("byte_count");
    QTest::addColumn<unsigned char>("data1");
    QTest::addColumn<unsigned char>("data2");
    QTest::addColumn<unsigned char>("data3");
    QTest::addColumn<unsigned char>("data4");
    QTest::addColumn<unsigned char>("data5");
    QTest::addColumn<unsigned char>("data6");

    QTest::newRow("0") << (unsigned char) 0x01 << "\x03" << (unsigned char) 0x06
                       << (unsigned char) 0xCC << (unsigned char) 0xFD
                       << (unsigned char) 0xDD << (unsigned char) 0xEE
                       << (unsigned char) 0x55 << (unsigned char) 0x52;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x03" << (unsigned char) 0x06
                       << (unsigned char) 0x51 << (unsigned char) 0x56
                       << (unsigned char) 0x52 << (unsigned char) 0x55
                       << (unsigned char) 0x53 << (unsigned char) 0x54;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x04" << (unsigned char) 0x06
                       << (unsigned char) 0x34 << (unsigned char) 0x33
                       << (unsigned char) 0x35 << (unsigned char) 0x32
                       << (unsigned char) 0x36 << (unsigned char) 0x31;
    QTest::newRow("3") << (unsigned char) 0x01 << "\x04" << (unsigned char) 0x06
                       << (unsigned char) 0x40 << (unsigned char) 0x40
                       << (unsigned char) 0x41 << (unsigned char) 0x41
                       << (unsigned char) 0x41 << (unsigned char) 0x40;
}


void ModbusTest::f7_11_12_Snd(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);

    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    Message* mesg = new Message(function, "");
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);
    //qDebug() << "No to czytam!";
    //qDebug() << "hejho!";
    unsigned char* answer = new unsigned char[2];
    if ((readed = read(fd, answer, 2)) < 0)
        qDebug() << "Read error!" << errno;
    //if ((readed = read(fd, &crc, sizeof(short))) < 0)
    //    qDebug() << "CRC read error";
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[0]);
    delete[] answer;
}

void ModbusTest::f7_11_12_Snd_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x07";
    QTest::newRow("1") << (unsigned char) 0x02 << "\x07";
    QTest::newRow("2") << (unsigned char) 0x03 << "\x0B";
    QTest::newRow("3") << (unsigned char) 0x02 << "\x0B";
    QTest::newRow("4") << (unsigned char) 0x01 << "\x0C";
}


void ModbusTest::f5_6_11_15_Rec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, data1);
    QFETCH(unsigned char, data2);
    QFETCH(unsigned char, data3);
    QFETCH(unsigned char, data4);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    unsigned char* response = new unsigned char[6];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = data1;
    response[3] = data2;
    response[4] = data3;
    response[5] = data4;
    QString tmp;
    tmp.append(data1).append(data2).append(data3).append(data4).append('\0');
    crc = qChecksum((char*) response, 6);
    if ((readed = write(fd, response, 6)) < 0)
        qDebug() << "Test: Write error!";
    if ((readed = write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
    QCOMPARE(tmp, returned[0].value);
}

void ModbusTest::f5_6_11_15_Rec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned char>("data1");
    QTest::addColumn<unsigned char>("data2");
    QTest::addColumn<unsigned char>("data3");
    QTest::addColumn<unsigned char>("data4");

    QTest::newRow("0") << (unsigned char) 0x01 << "\x05"
                       << (unsigned char) 0xCC << (unsigned char) 0xFD
                       << (unsigned char) 0xDD << (unsigned char) 0xEE;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x05"
                       << (unsigned char) 0x52 << (unsigned char) 0x54
                       << (unsigned char) 0x51 << (unsigned char) 0x53;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x06"
                       << (unsigned char) 0xA3 << (unsigned char) 0xFF
                       << (unsigned char) 0x43 << (unsigned char) 0xFC;
    QTest::newRow("3") << (unsigned char) 0x01 << "\x06"
                       << (unsigned char) 0x41 << (unsigned char) 0xFD
                       << (unsigned char) 0x55 << (unsigned char) 0x45;
    QTest::newRow("4") << (unsigned char) 0x01 << "\x0B"
                       << (unsigned char) 0x23 << (unsigned char) 0x24
                       << (unsigned char) 0x26 << (unsigned char) 0x25;
    QTest::newRow("5") << (unsigned char) 0x01 << "\x0F"
                       << (unsigned char) 0xA4 << (unsigned char) 0xF6
                       << (unsigned char) 0xC4 << (unsigned char) 0xB4;

}

void ModbusTest::f7_Rec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, data1);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    unsigned char* response = new unsigned char[3];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = data1;
    QString tmp;
    tmp.append(data1).append('\0');
    crc = qChecksum((char*) response, 3);
    if ((readed = write(fd, response, 3)) < 0)
        qDebug() << "Test: Write error!";
    if ((readed = write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
    QCOMPARE(tmp, returned[0].value);
}

void ModbusTest::f7_Rec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned char>("data1");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x07"
                       << (unsigned char) 0xCC;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x07"
                       << (unsigned char) 0xCC;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x07"
                       << (unsigned char) 0xCC;
}

void ModbusTest::f0C_Rec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, byte_count);
    QFETCH(unsigned char, data1);
    QFETCH(unsigned char, data2);
    QFETCH(unsigned char, data3);
    QFETCH(unsigned char, data4);
    QFETCH(unsigned char, data5);
    QFETCH(unsigned char, data6);
    QFETCH(unsigned char, data7);
    QFETCH(unsigned char, data8);
    QFETCH(unsigned char, data9);
    QFETCH(unsigned char, data10);
    QFETCH(unsigned char, data11);
    QFETCH(unsigned char, data12);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    unsigned char* response = new unsigned char[15];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = byte_count;
    response[3] = data1;
    response[4] = data2;
    response[5] = data3;
    response[6] = data4;
    response[7] = data5;
    response[8] = data6;
    response[9] = data7;
    response[10] = data8;
    response[11] = data9;
    response[12] = data10;
    response[13] = data11;
    response[14] = data12;
    QString tmp;
    tmp.append(data1).append(data2).append(data3).append(data4).append(data5)
       .append(data6).append(data7).append(data8).append(data9).append(data10)
       .append(data11).append(data12).append('\0');
    crc = qChecksum((char*) response, 15);
    if ((readed = write(fd, response, 15)) < 0)
        qDebug() << "Test: Write error!";
    if ((readed = write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
    QCOMPARE(tmp, returned[0].value);
}

void ModbusTest::f0C_Rec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned char>("byte_count");
    QTest::addColumn<unsigned char>("data1");
    QTest::addColumn<unsigned char>("data2");
    QTest::addColumn<unsigned char>("data3");
    QTest::addColumn<unsigned char>("data4");
    QTest::addColumn<unsigned char>("data5");
    QTest::addColumn<unsigned char>("data6");
    QTest::addColumn<unsigned char>("data7");
    QTest::addColumn<unsigned char>("data8");
    QTest::addColumn<unsigned char>("data9");
    QTest::addColumn<unsigned char>("data10");
    QTest::addColumn<unsigned char>("data11");
    QTest::addColumn<unsigned char>("data12");

    QTest::newRow("0") << (unsigned char) 0x01 << "\x0C" << (unsigned char) 0x0C
                       << (unsigned char) 0xCC << (unsigned char) 0xFD
                       << (unsigned char) 0xDD << (unsigned char) 0xEE
                       << (unsigned char) 0x55 << (unsigned char) 0x52
                       << (unsigned char) 0xCC << (unsigned char) 0xFD
                       << (unsigned char) 0xDD << (unsigned char) 0xEE
                       << (unsigned char) 0x55 << (unsigned char) 0x52;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x0C" << (unsigned char) 0x0C
                       << (unsigned char) 0x50 << (unsigned char) 0x49
                       << (unsigned char) 0x51 << (unsigned char) 0x48
                       << (unsigned char) 0x52 << (unsigned char) 0x47
                       << (unsigned char) 0x53 << (unsigned char) 0x46
                       << (unsigned char) 0x54 << (unsigned char) 0x45
                       << (unsigned char) 0x55 << (unsigned char) 0x44;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x0C" << (unsigned char) 0x0C
                       << (unsigned char) 0xBB << (unsigned char) 0xCC
                       << (unsigned char) 0x11 << (unsigned char) 0xEE
                       << (unsigned char) 0x56 << (unsigned char) 0xFC
                       << (unsigned char) 0xCC << (unsigned char) 0xFA
                       << (unsigned char) 0xDD << (unsigned char) 0x5C
                       << (unsigned char) 0xB5 << (unsigned char) 0x52;
}
