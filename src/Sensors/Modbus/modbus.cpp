#include "modbus.h"
#include "modbusrtuframe.h"

#include <iostream>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <errno.h>


Modbus* Modbus::create(Configurator *config){
    // TODO
    return new Modbus(config);
}

Modbus* Modbus::clone(Configurator *config){
    // TODO
    return new Modbus(config);
}

Modbus::Modbus(Configurator* new_config){
        config = new_config;
        // Change after defining config
        preparePort("/dev/pts/3");
        portNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
        QObject::connect(portNotifier, SIGNAL(activated()), this, SLOT(readFromSensor()));
}


int Modbus::preparePort(std::string port){
        // Check params
        if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
            qDebug() << "Open port failure" << fd;
        std::string port2("/dev/pts/5");
        if ((fd2 = open(port2.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
            qDebug() << "Open port2 failure" << errno;
        termios* port_param = new termios;
        port_param->c_cflag = B9600 | CS8;
        port_param->c_iflag = IGNPAR;
        port_param->c_oflag = 0;
        port_param->c_lflag = 0;
        port_param->c_cc[VMIN] = 1;
        port_param->c_cc[VTIME] = 0;
        if (tcsetattr(fd, TCSAFLUSH, port_param) < 0)
                qDebug() << "Setting port attr error";
        tcsetattr(fd2, TCSAFLUSH, port_param);
        delete port_param;
        return 0;
}

int Modbus::connect(unsigned char addr){
        sensor_addr = addr;
        return 0;
}

//Add more function codes
ModbusRtuFrame* Modbus::decodeMessage(Message msg){
    ModbusRtuFrame* frame;
    unsigned char* data;
    if (msg.key == "1") { // read coils
        frame = new ModbusRtuFrame('1', 4);
        data = new unsigned char[4];
        data[0] = 0;
        data[1]&0xFF;
        data[2] = 0;
        data[3]&0xFF;
        frame->setData(data, 4);
    } else if (msg.key == "2") { //read discrete
        frame = new ModbusRtuFrame('2', 4);
        data = new unsigned char[4];
        data[0]&0xFF;
        data[1]&0xFF;
        data[2] = 0;
        data[3]&0xFF;
        frame->setData(data, 4);
    } else if (msg.key == "3") { // read holding registers
        frame = new ModbusRtuFrame('3', 4);
        data = new unsigned char[4];
        data[0] = 0;
        data[1]&0xFF;
        data[2] = 0;
        data[3] = 1; //&0x7D;
        frame->setData(data, 4);
    } else if (msg.key == "4") {
        frame = new ModbusRtuFrame('4', 4);
        data = new unsigned char[4];
        data[0] = 0;
        data[1]&0xFF;
        data[2] = 0;
        data[3] = 1; //&0x7D;
        frame->setData(data, 4);
    } else if (msg.key == "43") { // read device id
        frame = new ModbusRtuFrame(0x2B, 3);
        data = new unsigned char[3];
        data[0] = 0x0E;
        data[1] = 0x01;
        data[2] = 0;
        frame->setData(data, 3);
    }
    delete []data;
    frame->setAddr(sensor_addr);
    return frame;
}

void Modbus::write( QVector<Message> messages){
        for (QVector<Message>::iterator it = messages.begin(); it < messages.end(); it++) {
                ModbusRtuFrame* frame = decodeMessage(*it);
                unsigned char** data = frame->toSend();
                int temp = 0;
                for (int j = 0; j < 2; j++) {
                    if ((temp = ::write(fd, data[j], sizeof(j))) < 0)
                        qDebug() << "write error!" << errno;
                }
                if ((temp = ::write(fd, data[2], frame->showSize())) < 0)
                    qDebug() << "data write error";
                if ((temp = ::write(fd, data[3], sizeof(unsigned short))) < 0)
                    qDebug() << "crc write error";
                delete []data;
                delete frame;
        }
}


QVector<Message> Modbus::readAll(){
    QVector<Message> messages = QVector<Message>(msgQue);
    msgQue.clear();
    return messages;
}


void Modbus::readFromSensor(){
    unsigned char* answer = new unsigned char[2];
    unsigned char* answer_data;
    int readed;
    if ((readed = read(fd2, answer, 2)) < 0)
        qDebug() << "Read error!" << errno; // czytamy adres i funkcje
    unsigned char bytes;
    if (answer[1] == '1') {
        read(fd, &bytes, 1);
        answer_data = new unsigned char[(int) bytes];
        read(fd, answer_data, (int) bytes);
    }
    qDebug() << answer[0];
    qDebug() << answer[1];
    qDebug() << answer[2];
    //unsigned char* odpowiedz = (unsigned char*) answer;
    //Message mesg("chciany klucz", QString(answer_data[0]));
    //msgQue.push_back(mesg);
    delete []answer;
    //delete []answer_data;
}

Modbus::~Modbus(){
        if ((close(fd)) < 0)
            qDebug() << "Couldn't close file descriptor";
        delete portNotifier;
}
