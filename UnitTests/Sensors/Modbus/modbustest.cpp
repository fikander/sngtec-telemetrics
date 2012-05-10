#include "modbustest.h"
#include "src/Sensors/Modbus/modbus.h"
#include "src/Sensors/Modbus/crc-count.h"
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

unsigned short ModbusTest::countCRC(unsigned char *array, int array_length){
    return modbusCRC((unsigned char*) array, array_length);
}

void ModbusTest::testPort(){
    std::string port2("/dev/pts/3");
    if ((fd = open(port2.c_str(), O_RDWR | O_NOCTTY)) < 0) //wywalamy O_NONBLOCK, wtedy dziala
        qDebug() << "ModbusTest: Open test port failure" << "Errno: " << errno;
    termios* port_param = new termios;
    port_param->c_cflag = B9600 | CS8;
    port_param->c_iflag = IGNPAR;
    port_param->c_oflag = 0;
    port_param->c_lflag = 0;
    port_param->c_cc[VMIN] = 1;
    port_param->c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, port_param) < 0)
        qDebug() << "ModbusTest: Setting test port attr error";
    delete[] port_param;
}

//Rozdzielamy testy write/read



void ModbusTest::f1_2_3_4_Snd()
{
    qDebug() << "Cos nie halo z tabami!!";
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(QString, msg_data);
    testPort();
    Modbus* m = new Modbus(new Configurator());

    Message* mesg = new Message(function, msg_data);
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);

    unsigned char* answer = new unsigned char[6];
    if ((readed = read(fd, answer, 2)) < 0)
        qDebug() << "Read error!" << errno; // czytamy adres i funkcje
    if ((readed = read(fd, &answer[2], 4)) < 0)
        qDebug() << "Read error!" << errno;
    if ((readed = read(fd, &crc, sizeof(short))) < 0)
        qDebug() << "CRC read error";
    unsigned short my_crc = countCRC(answer, 6);

    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[1]);
    QCOMPARE(answer[5], (unsigned char) msg_data.at(3).toAscii());
    QCOMPARE(my_crc, crc);
    delete[] answer;

}

void ModbusTest::f1_2_3_4_Snd_data()
{
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<QString>("msg_data");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x01\x01" << "\x52\x4B\x11\x11\x34\x67";
    QTest::newRow("1") << (unsigned char) 0x02 << "\x02\x02" << "1342432";
    QTest::newRow("2") << (unsigned char) 0x03 << "\x03\x03" << "324352";
    QTest::newRow("3") << (unsigned char) 0x02 << "\x02\x01" << "125243";
    QTest::newRow("4") << (unsigned char) 0x01 << "\x01\x01" << "4234231";
}


void ModbusTest::errorRec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, error);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    unsigned char* response = new unsigned char[3];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = error;
    crc = countCRC(response, 3);
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
    crc = countCRC(response, 9);
    qDebug() << "cos siada!";
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

    Message* mesg = new Message(function, "");
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);

    unsigned char* answer = new unsigned char[2];
    if ((readed = read(fd, answer, 2)) < 0)
        qDebug() << "Read error!" << errno;
    if ((readed = read(fd, &crc, sizeof(short))) < 0)
        qDebug() << "CRC read error";
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[1]);
    delete[] answer;
}

void ModbusTest::f7_11_12_Snd_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x01\x07";
    QTest::newRow("1") << (unsigned char) 0x02 << "\x02\x07";
    QTest::newRow("2") << (unsigned char) 0x03 << "\x03\x0B";
    QTest::newRow("3") << (unsigned char) 0x02 << "\x02\x0B";
    QTest::newRow("4") << (unsigned char) 0x01 << "\x01\x0C";
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

    unsigned char* response = new unsigned char[3];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = data1;
    QString tmp;
    tmp.append(data1); //.append('\0');
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


void ModbusTest::f8_Rec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned char, data1);
    QFETCH(unsigned char, data2);
    QFETCH(unsigned char, data3);
    QFETCH(unsigned char, data4);
    testPort();
    Modbus* m = new Modbus(new Configurator());

    unsigned char* response = new unsigned char[6];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = data1;
    response[3] = data2;
    response[4] = data3;
    response[5] = data4;
    crc = qChecksum((char*) response, 6);
    if ((readed = write(fd, response, 6)) < 0)
        qDebug() << "Test: Write error!";
    if ((readed = write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
}

void ModbusTest::f8_Rec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned char>("data1");
    QTest::addColumn<unsigned char>("data2");
    QTest::addColumn<unsigned char>("data3");
    QTest::addColumn<unsigned char>("data4");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x08"
                       << (unsigned char) 0x00 << (unsigned char) 0x01
                       << (unsigned char) 0xCC << (unsigned char) 0xCC;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x08"
                       << (unsigned char) 0x00 << (unsigned char) 0x01
                       << (unsigned char) 0xCC << (unsigned char) 0xCC;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x08"
                       << (unsigned char) 0xCC << (unsigned char) 0xCC
                       << (unsigned char) 0xCC << (unsigned char) 0xCC;
}

void ModbusTest::f8_Snd(){
    //porownanie CRC?
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(QString, data);
    testPort();
    Modbus* m = new Modbus(new Configurator());

    Message* mesg = new Message(function, data);
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);
    unsigned char* answer = new unsigned char[6];
    if ((readed = read(fd, answer, 6)) < 0)
        qDebug() << "Read error!" << errno;
    if ((readed = read(fd, &crc, sizeof(short))) < 0)
        qDebug() << "CRC read error";
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[1]);
    QCOMPARE(answer[3], (unsigned char) data.toAscii()[2]);
    delete[] answer;
}

void ModbusTest::f8_Snd_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x01\x08" << "\00\01\02\03";
    QTest::newRow("1") << (unsigned char) 0x01 << "\x01\x08" << "\00\04\02\03";
    QTest::newRow("2") << (unsigned char) 0x01 << "\x01\x08" << "\00\08\04\03";
}

void ModbusTest::f18_Rec(){
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(unsigned short, byte_count);
    QFETCH(unsigned char, data1);
    QFETCH(unsigned char, data2);
    QFETCH(unsigned char, data3);
    QFETCH(unsigned char, data4);
    QFETCH(unsigned char, data5);
    QFETCH(unsigned char, data6);
    testPort();
    Modbus* m = new Modbus(new Configurator());

    unsigned char* response = new unsigned char[10];
    response[0] = addr;
    response[1] = function.at(0).toAscii();
    response[2] = 0x06;
    response[3] = 0x00;
    response[4] = data1;
    response[5] = data2;
    response[6] = data3;
    response[7] = data4;
    response[8] = data5;
    response[9] = data6;
    QString tmp;
    tmp.append(data1).append(data2).append(data3).append(data4).append(data5).append(data6).append('\0');
    crc = countCRC(response, 10);
    if ((write(fd, response, 10)) < 0)
        qDebug() << "Test: Write error!";
    if ((write(fd, &crc, sizeof(short))) < 0)
        qDebug() << "Test: CRC write error";
    m->readFromSensor();
    QVector<Message> returned = m->readAll();
    QCOMPARE(tmp, returned[0].value);
}



void ModbusTest::f18_Rec_data(){
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<unsigned short>("byte_count");
    QTest::addColumn<unsigned char>("data1");
    QTest::addColumn<unsigned char>("data2");
    QTest::addColumn<unsigned char>("data3");
    QTest::addColumn<unsigned char>("data4");
    QTest::addColumn<unsigned char>("data5");
    QTest::addColumn<unsigned char>("data6");

    QTest::newRow("0") << (unsigned char) 0x01 << "\x18" << (unsigned short) 6
                       << (unsigned char) 0xCC << (unsigned char) 0xFD
                       << (unsigned char) 0xDD << (unsigned char) 0xEE
                       << (unsigned char) 0x55 << (unsigned char) 0x52;
    QTest::newRow("1") << (unsigned char) 0x01 << "\x18" << (unsigned short) 6
                       << (unsigned char) 0x51 << (unsigned char) 0x56
                       << (unsigned char) 0x52 << (unsigned char) 0x55
                       << (unsigned char) 0x53 << (unsigned char) 0x54;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x18" << (unsigned short) 6
                       << (unsigned char) 0x34 << (unsigned char) 0x33
                       << (unsigned char) 0x35 << (unsigned char) 0x32
                       << (unsigned char) 0x36 << (unsigned char) 0x31;
    QTest::newRow("3") << (unsigned char) 0x01 << "\x18" << (unsigned short) 6
                       << (unsigned char) 0x40 << (unsigned char) 0x40
                       << (unsigned char) 0x41 << (unsigned char) 0x41
                       << (unsigned char) 0x41 << (unsigned char) 0x40;
}

void ModbusTest::f0F_10_Snd() {
    //porownanie CRC?
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    QFETCH(QString, msg_data);
    QFETCH(int, size);
    testPort();
    Modbus* m = new Modbus(new Configurator());

    Message* mesg = new Message(function, msg_data);
    //qDebug() << "s" << msg_data.size();
    //qDebug() << "Ma byc val:" << mesg->value;
    //qDebug() << "do tego: " << mesg->value.at(0) << mesg->value.size();
    //qDebug() << "Array: " << mesg->value.toAscii().at(5);
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);
    unsigned char* answer = new unsigned char[size];
    if ((read(fd, answer, size)) < 0)
        qDebug() << "Read error!" << errno;
    if ((read(fd, &crc, sizeof(short))) < 0)
        qDebug() << "CRC read error";
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[1]);
    QCOMPARE(answer[4], (unsigned char) msg_data.toAscii().at(2));
    delete[] answer;
}

void ModbusTest::f0F_10_Snd_data() {
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<QString>("msg_data");
    QTest::addColumn<int>("size");
    QTest::newRow("0") << (unsigned char) 0x01 << "\x01\x0F" <<
                          QString::fromAscii("\x55\x54\x55\xFF\x02\x4B\x50", 7) << 9;
    QTest::newRow("1") << (unsigned char) 0x02 << "\x02\x0F" <<
                          QString::fromAscii("\x55\x54\x00\x03\x04\x56\x57\x68\x66", 9) << 11;
    QTest::newRow("2") << (unsigned char) 0x01 << "\x01\x10" <<
                          QString::fromAscii("\x00\x08\x04\x03\x03\x65\x56\x56", 8) << 10;
    QTest::newRow("3") << (unsigned char) 0x01 << "\x01\x14" <<
                          QString::fromAscii("\x0A\x55\x00\xFF\x52\x4B\x50\x53\x32\x23\x4D", 11) << 13;
    QTest::newRow("4") << (unsigned char) 0x02 << "\x02\x14" <<
                          QString::fromAscii("\x08\x54\x00\x03\x04\x56\x57\x68\x66", 9) << 11;
    QTest::newRow("5") << (unsigned char) 0x01 << "\x01\x15" <<
                          QString::fromAscii("\x0A\x08\x04\x03\x03\x65\x56\x56\x65\x34\xBA", 11) << 13;
    QTest::newRow("6") << (unsigned char) 0x01 << "\x01\x16" <<
                          QString::fromAscii("\x0A\x08\x04\x03\x03\x65", 6) << 8;
    QTest::newRow("7") << (unsigned char) 0x01 << "\x01\x16" <<
                          QString::fromAscii("\x0A\x08\xFF\xCF\x4D\x65", 6) << 8;
    QTest::newRow("8") << (unsigned char) 0x01 << "\x01\x17" <<
                          QString::fromAscii("\xCD\x08\x04\x03\x03\x65\x56\x56\x08\xBA\xBA\xBA\xBA\xBA\xBA\xBA\xBA", 17) << 19;
    QTest::newRow("9") << (unsigned char) 0x01 << "\x01\x17" <<
                          QString::fromAscii("\x0A\x08\x04\x03\x03\x65\x56\x56\x06\xBA\x56\x56\x56\x56\x56", 15) << 17;
}
