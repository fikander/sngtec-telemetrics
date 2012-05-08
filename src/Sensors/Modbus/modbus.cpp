#include "modbus.h"
#include "modbusrtuframe.h"

#include "Sensors/Modbus/crc-count.h"

#include <iostream>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <errno.h>
#include <string>




const std::string serial_port_name = "/dev/ttyS0";

Modbus* Modbus::create(Configurator *config, int no){
    return new Modbus(config);
}

Modbus* Modbus::clone(Configurator *config, int no){

    return new Modbus(config);
}

Modbus::Modbus(Configurator* new_config){
        config = new_config;
        // numer portu z konfiguratora!
        preparePort(serial_port_name); //.toStdString().c_str());
        portNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
        QObject::connect(portNotifier, SIGNAL(activated(int)), this, SLOT(readFromSensor()), Qt::DirectConnection);
}


int Modbus::preparePort(std::string port){
        if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY)) < 0)
            qDebug() << "Modbus: Opening port " << fd << " failure. Errno: " << errno << " !!!";
        termios* port_param = new termios;
        port_param->c_cflag = B9600 | CS8 | PARENB | CREAD | CLOCAL;
        port_param->c_iflag = IGNBRK | IGNPAR;
        port_param->c_oflag = 0;
        port_param->c_lflag = 0;
        port_param->c_cc[VMIN] = 1;
        port_param->c_cc[VTIME] = 0;
        if (tcsetattr(fd, TCSAFLUSH, port_param) < 0)
            qDebug() << "Modbus: Setting port attr error. Errno: " << errno << " !!!";
        delete port_param;
        return 0;
}


ModbusRtuFrame* Modbus::decodeMessage(Message msg){
    ModbusRtuFrame* frame = NULL;
    unsigned char* data = NULL;
    unsigned char sensor_address = msg.key.at(0).toAscii();
    msg.key = msg.key.at(1);
            //read coils / discrete/ holding registers/ input registers
    if ((msg.key == "\x01") || (msg.key == "\x02") || (msg.key == "\x03") || (msg.key == "\x04") ||
            (msg.key == "\x05") || (msg.key == "\x06")) {
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), 4);
        frame->setData(msg.value.toAscii());
        qDebug() << "To jest jako data: " << frame->getData()[0] << " " << frame->getData()[1]
                 << " " << frame->getData()[2] << " " << frame->getData()[3] << " ";
    } else if ((msg.key == "\x07") || (msg.key == "\x0B") || (msg.key == "\x0C")) {
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), 0);
    } else if (msg.key == "\x08") { // diagnostic
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), 4);
        data = new unsigned char[4];
        if (msg.value.at(1).toAscii() == '\x00') {
            data[0] = msg.value.at(0).toAscii();
            data[1] = msg.value.at(1).toAscii();
            data[2] = '\x54';
            data[3] = '\x65';
        } else {
            data[0] = msg.value.at(0).toAscii();
            data[1] = msg.value.at(1).toAscii();
            data[2] = '\x00';
            data[3] = '\x00';
        }
        frame ->setData(data,4);
        delete []data;
    } else if ((msg.key == "\x0F") || (msg.key == "\x10")) {
        char size = msg.value.at(4).toAscii();
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), ((int)(size)) + 4 + 1);
        frame->setData(msg.value.toAscii());
    } else if ((msg.key == "\x14") || (msg.key == "\x15")){ //read file record
        char byte_count = msg.value.at(0).toAscii();
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), ((int)byte_count) + 1);
        frame->setData(msg.value.toAscii());
    } else if (msg.key == "\x16") { // mask
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), 6);
        frame->setData(msg.value.toAscii());
    } else if (msg.key == "\x17") { //read/write multiple
        char byte_count = msg.value.at(8).toAscii();
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), byte_count + 9);
        frame->setData(msg.value.toAscii());
    } else if (msg.key == "\x18") {
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), 2);
        frame->setData(msg.value.toAscii());
    }
    //delete []data;
    frame->setAddr(sensor_address);
    qDebug() << "koniec decoda";
    return frame;
}

void Modbus::write( QVector<Message> messages){
        for (QVector<Message>::iterator it = messages.begin(); it < messages.end(); it++) {
                ModbusRtuFrame* frame = decodeMessage(*it);
                unsigned char** data = frame->toSend();
                for (int j = 0; j < 2; j++) {
                    if ((::write(fd, data[j], 1)) < 0)
                        qDebug() << "Modbus: write error!" << errno;
                }
                if ((::write(fd, data[2], frame->showSize())) < 0)
                    qDebug() << "Modbus: data write error";
                if ((::write(fd, data[3], sizeof(short))) < 0)
                    qDebug() << "Modbus: crc write error";
                delete []data;
                delete frame;
                ::sleep(1);
        }
        qDebug() << "Modbus writing finished ";
}


QVector<Message> Modbus::readAll(){
    QVector<Message> messages = QVector<Message>(msgQue);
    msgQue.clear();
    return messages;
}



void Modbus::readFromSensor(){
    // answer[0] = address, [1] = function, [2] = byte count
    unsigned char* answer = new unsigned char[3];
    unsigned char* answer_data = NULL;
    unsigned short crc;
    unsigned short sent_crc;
    short take_byte_count = 0; // whether answer[2] is active
    short answer_size = 0;
    if ((read(fd, answer, 2)) < 2)
        qDebug() << "Modbus: Read error!" << errno;
    qDebug() << "Modbus: incoming transmission";
    if (answer[1] < 0x80) { // normal function
        if ((answer[1] == 0x01) || (answer[1] == 0x02) || (answer[1] == 0x03) ||
                (answer[1] == 0x04) || (answer[1] == 0x0C) || (answer[1] == 0x14) ||
                (answer[1] == 0x15) || (answer[1] == 0x17)) {
            qDebug() << (int) answer[0] << (int) answer[1] << "petla pierwsza!!!";
            read(fd, &answer[2], 1);
            answer_size = answer[2];
            answer_data = new unsigned char[answer_size];
            if (read(fd, answer_data, answer_size) < 0)
                qDebug() << "Modbus: Read error!" << errno;
            take_byte_count = 1;
            qDebug() << "answer size: " << answer_size;
            qDebug() << "answer: ";
            for (int z=0; z<answer_size; z++){
                qDebug() << answer_data[z] << z;
            }
        } else if ((answer[1] == 0x05) || (answer[1] == 0x06) || (answer[1] == 0x0B) ||
                   (answer[1] == 0x0F) || (answer[1] == 0x10)) {
            answer_data = new unsigned char[5];
            //qDebug() << "received function 2";
            answer_size = 4;
            read(fd, answer_data, answer_size);
            answer_data[4] = '\0';
            take_byte_count = 0;
        } else if (answer[1] == 0x07) {
            //qDebug() << "Modbus: received function 7";
            answer_data = new unsigned char[2];
            answer_size = 1;
            read(fd, answer_data, answer_size);
            answer_data[1] = '\0';
            take_byte_count = 0;
        } else if (answer[1] == 0x08) {
            //qDebug() << "Modbus: received function 8";
            answer_data = new unsigned char[5];
            answer_size = 4;
            read(fd, answer_data, answer_size);
            answer_data[4] = '\0';
            if ((answer_data[1] == '\x00') && ((answer_data[2] != '\x54') || (answer_data[3] != '\x65'))) {
                qDebug() << "Modbus: Function 0x08 with subfunction 0000 returned and failed!";
                qDebug() << "Modbus: Wrong data returned by sensor";
            } else {
                qDebug() << "Modbus: Function 0x08 with subfunction 00" << answer_data[1] << " returned! ";
            }
        } else if (answer[1] == 0x16) {
            //qDebug() << "Modbus: received function 16";
            answer_data = new unsigned char[7];
            answer_size = 6;
            read(fd, answer_data, answer_size);
            answer_data[6] = '\0';
            take_byte_count = 0;
        } else if ((answer[1] == 0x18)) { // FIFO
            //qDebug() << "Modbus: received function 18";
            read(fd, &answer_size, 2);
            answer_data = new unsigned char[answer_size + 1];
            read(fd, answer_data, answer_size);
            answer_data[answer_size] = '\0';
            take_byte_count = 2;
        }
        qDebug() << "Modbus: reading CRC ...";
        read(fd, &sent_crc, sizeof(short));
        if(checkResponseCRC(answer, answer_data, answer_size, take_byte_count, sent_crc)) {
            if (answer[1] != 0x08) {
                QString* ret = new QString();
                (*ret).append(answer[0]).append(answer[1]);
                Message mesg(*ret, QString::fromAscii((char *) answer_data, answer_size));
                msgQue.push_back(mesg);
                qDebug() << "Z koleiki: " << msgQue[0].value;
                delete ret;
            }
           // qDebug() << "CRC of received message is ok!";
        } else
            qDebug() << "Modbus: CRC of received data is wrong!";
        delete []answer_data;
    } else { // error arrived
        read(fd, &answer[2], 1);
        read(fd, &sent_crc, sizeof(short));
        crc = modbusCRC((unsigned char*) answer, 3);
        if (crc != sent_crc)
            qDebug () << "Modbus: CRC of received data is wrong!";
        else {
            qDebug() << "Modbus: Sensor send error!";
            qDebug() << "Modbus: Sensor addr : " << answer[0];
            qDebug() << "Modbus: Error code : " << answer[1];
            qDebug() << "Modbus: Exception code : " << answer[2];
        }
    }
    //qDebug() << "Basta!";
    delete []answer;
}

int Modbus::checkResponseCRC(unsigned char* answer, unsigned char* answer_data,
                             int answer_size, int take_byte_count,unsigned short crc) {
    int ret = 0;
    unsigned char* tmp = new unsigned char[answer_size + 1 + 3 + 1];
    memcpy(tmp, answer, 3);
    if (! take_byte_count) {
        memcpy(&tmp[2], answer_data, answer_size);
        answer_size--;
    } else if (take_byte_count == 2){ //for FIFO
        memcpy(&tmp[2], &answer_size, sizeof(short));
        memcpy(&tmp[4], answer_data, answer_size);
        answer_size++;
    } else {
        memcpy(&tmp[3], answer_data, answer_size);
    }
    tmp[answer_size + 3] = '\0';
    //qDebug() << "crc" << crc << "obliczone:" << modbusCRC((unsigned char*) tmp, answer_size + 3) << answer_size;
    if (crc == modbusCRC((unsigned char*) tmp, answer_size + 3))
        ret = 1;
    delete[] tmp;
    return ret;
}


Modbus::~Modbus(){
        if ((close(fd)) < 0)
            qDebug() << "Couldn't close file descriptor";
        delete portNotifier;
}
