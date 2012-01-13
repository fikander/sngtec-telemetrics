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
    std::string port2("/dev/pts/5");
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

void ModbusTest::simpleTest()
{
    QFETCH(unsigned char, addr);
    QFETCH(QString, function);
    testPort();
    Modbus* m = new Modbus(new Configurator());
    m->connect(addr);
    Message* mesg = new Message(function, "0");
    QVector<Message>* messages = new QVector<Message>();
    messages->append(*mesg);
    m->write(*messages);
    // zczytujemy co zapisał
    unsigned char* answer = new unsigned char[2];
    int readed;
    if ((readed = read(fd, answer, 2)) < 0)
        qDebug() << "Read error!" << errno; // czytamy adres i funkcje
    qDebug() << answer;
    QCOMPARE(answer[0], addr);
    QCOMPARE(answer[1], (unsigned char) function.toStdString()[0]);
    delete[] answer;
}

void ModbusTest::simpleTest_data()
{
    QTest::addColumn<unsigned char>("addr");
    QTest::addColumn<QString>("function");
    QTest::newRow("0") << (unsigned char) '2' << "1";
    QTest::newRow("1") << (unsigned char) '1' << "2";
    QTest::newRow("2") << (unsigned char) '3' << "1";
    QTest::newRow("3") << (unsigned char) '2' << "2";
    QTest::newRow("4") << (unsigned char) '2' << "3";
}
