#include "modbustest.h"
#include "src/Sensors/Modbus/modbus.h"
#include "src/Configurator/configurator.h"
#include "src/Message/message.h"

#include <QVector>
#include <QDebug>
#include <termios.h>
#include <string>
#include <errno.h>

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

// test wysylania funkcji 1
void ModbusTest::simpleTest()
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
    unsigned short crc;
    int readed;
    if ((readed = read(fd, answer, 2)) < 0)
        qDebug() << "Read error!" << errno; // czytamy adres i funkcje
    if ((readed = read(fd, data, 4)) < 0)
        qDebug() << "Read error!" << errno;
    if ((readed = read(fd, &crc, sizeof(short))) < 0)
        qDebug() << "CRC read error";
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[0]);
    QCOMPARE(data[3], (unsigned char) NSize.toShort());
    delete[] answer;
    delete[] data;
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
    qDebug() << returned[0].value;
}

void ModbusTest::simpleTest_data()
{
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::addColumn<QString>("NSize");
    QTest::addColumn<unsigned char>("testval");
    QTest::newRow("0") << (unsigned char) 0x02 << "1" << "5" << (unsigned char) 0xBB;
    QTest::newRow("1") << (unsigned char) 0x01 << "1" << "12" << (unsigned char) 0x3F;
    QTest::newRow("2") << (unsigned char) 0x03 << "1" << "32" << (unsigned char) 0xCC;
    QTest::newRow("3") << (unsigned char) 0x02 << "1" << "1" << (unsigned char) 0x45;
    QTest::newRow("4") << (unsigned char) 0x02 << "1" << "41" << (unsigned char) 0x03;
}
