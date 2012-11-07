#include "modbus_student.h"
#include "modbusrtuframe.h"

#include "Sensors/Modbus/crc-count.h"

#include <iostream>
#include <QByteArray>
#include <QVector>
#include <QStringList>
#include <QDebug>
#include <errno.h>
#include <string>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>


ModbusStudent* ModbusStudent::create(Configurator *config, int no){
    return new ModbusStudent(config, no);
}

ModbusStudent* ModbusStudent::clone(Configurator *config, int no){

    return new ModbusStudent(config, no);
}

ModbusStudent::ModbusStudent() {}

ModbusStudent::ModbusStudent(Configurator* new_config, int no){
        config = new_config;
        number = no;
        QString serial_port_name = config->deviceTranslate(no, QString("port"));
        QString bandwidth = config->deviceTranslate(no, "bandwidth");
        QString parity = config->deviceTranslate(no, "parity");
        preparePort(serial_port_name.toAscii().data(), bandwidth, parity);

        portNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
        QObject::connect(portNotifier, SIGNAL(activated(int)), this, SLOT(readFromSensor()), Qt::DirectConnection);
}


int ModbusStudent::preparePort(char* port, QString bandwidth, QString parity){
        if ((fd = open(port, O_RDWR | O_NOCTTY)) < 0)
            qDebug() << "Modbus: Opening port " << fd << " failure. Errno: " << errno << " !!!";
        termios* port_param = new termios;
        port_param->c_cflag = CS8 | PARENB | CREAD | CLOCAL;
        if (!bandwidth.compare(QString("600")))
            port_param->c_cflag = port_param->c_cflag | B600;
        else if (!bandwidth.compare(QString("1200")))
            port_param->c_cflag = port_param->c_cflag | B1200;
        else if (!bandwidth.compare(QString("1800")))
            port_param->c_cflag = port_param->c_cflag | B1800;
        else if (!bandwidth.compare(QString("2400")))
            port_param->c_cflag = port_param->c_cflag | B2400;
        else if (!bandwidth.compare(QString("4800")))
            port_param->c_cflag = port_param->c_cflag | B4800;
        else if (!bandwidth.compare(QString("9600")))
            port_param->c_cflag = port_param->c_cflag | B9600;
        else if (!bandwidth.compare(QString("19200")))
            port_param->c_cflag = port_param->c_cflag | B19200;
        else if (!bandwidth.compare(QString("38400")))
            port_param->c_cflag = port_param->c_cflag | B38400;
        else if (!bandwidth.compare(QString("57600")))
            port_param->c_cflag = port_param->c_cflag | B57600;
        else if (!bandwidth.compare(QString("115200")))
            port_param->c_cflag = port_param->c_cflag | B115200;
        else {
            qDebug() << "Unknown parity was set, using default - 9600";
            port_param->c_cflag = port_param->c_cflag | B9600;
        }
        if (!parity.compare(QString("Odd"))) //otherwise parity is even
            port_param->c_cflag = port_param->c_cflag | PARODD;
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



MessageSample ModbusStudent::decodeByConfig(MessageSample msg){
    QString newKey = hexTranslate(config->deviceTranslate(number, msg.key));
    QString newVal = hexTranslate(config->deviceTranslate(number, msg.value));
    return MessageSample(newKey, newVal);
}

QString ModbusStudent::hexTranslate(QString toTranslate){
    bool ok;
    int hex;
    QString properMessage;
    QStringList hexes = toTranslate.split("\\x");
    hexes.removeFirst();
    for (QStringList::Iterator it = hexes.begin(); it < hexes.end(); it++) {
        hex = (*it).toInt(&ok, 16);
        properMessage.append((char) hex);
    }
    return properMessage;
}


ModbusRtuFrame* ModbusStudent::decodeMessage(MessageSample msg){
    ModbusRtuFrame* frame = NULL;
    unsigned char* data = NULL;
    if (! ((config->deviceTranslate(number, "noHexes")).compare("True")) )
        msg = decodeByConfig(msg);
    unsigned char sensor_address = msg.key.at(0).toAscii();
    msg.key = msg.key.at(1);
            //read coils / discrete/ holding registers/ input registers
    if (    (msg.key == "\x01") || (msg.key == "\x02") || (msg.key == "\x03") || (msg.key == "\x04") ||
            (msg.key == "\x05") || (msg.key == "\x06")) {
        frame = new ModbusRtuFrame(msg.key.at(0).toAscii(), 4);
        frame->setData(msg.value.toAscii());
        //qDebug() << "To jest jako data: " << frame->getData()[0] << " " << frame->getData()[1]
        //         << " " << frame->getData()[2] << " " << frame->getData()[3] << " ";
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
    } else {
        qDebug() << "Modbus can't send message !";
        return NULL;
    }
    frame->setAddr(sensor_address);
    //qDebug() << "koniec decoda";
    return frame;
}

void ModbusStudent::write( QVector<MessageSample> messages){
        for (QVector<MessageSample>::iterator it = messages.begin(); it < messages.end(); it++) {
                ModbusRtuFrame* frame = decodeMessage(*it);
                if (frame != NULL) {
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
                    ::sleep(1); //important for RTU frames
                } else
                    qDebug() << "Got wrong message!";
        }
        qDebug() << "Modbus writing finished ";
}


QVector<MessageSample> ModbusStudent::readAll(){
    QVector<MessageSample> messages = QVector<MessageSample>(msgQue);
    msgQue.clear();
    return messages;
}


int ModbusStudent::tryRead(unsigned char* buffer, int size){
    fd_set set;
    struct timeval timeout;
    int read_val;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    read_val = select(fd + 1, &set, NULL, NULL, &timeout);
    if (read_val == -1) {
        qDebug() << "Modbus read error: " << errno << " ";
        return 0;
    } else if (read_val == 0) {
        qDebug() << "Modbus read timeout!";
        return 0;
    } else {
        read_val = read(fd, buffer, size);
        if (read_val < size) {
            if (read_val < 0)
                qDebug() << "Modbus read error: " << errno << " ";
            else
                qDebug() << "Modbus received to short answer!";
            return 0;
        } else //received good answer
            return 1;
    }
}

void ModbusStudent::readFromSensor(){
    //qDebug() <<"Mamy czytac!";
    // answer[0] = address, [1] = function, [2] = byte count
    unsigned char answer[3];
    unsigned char* answer_data = NULL;
    unsigned short crc;
    unsigned short sent_crc;
    short take_byte_count = 0; // whether answer[2] is active
    short answer_size = 0;

    if (!tryRead(answer, 2))
        return;
    //qDebug() << "Modbus: incoming transmission";
    if (answer[1] < 0x80) { // normal function
        if (    (answer[1] == 0x01) || (answer[1] == 0x02) || (answer[1] == 0x03) ||
                (answer[1] == 0x04) || (answer[1] == 0x0C) || (answer[1] == 0x14) ||
                (answer[1] == 0x15) || (answer[1] == 0x17)) {

            if (!tryRead(&answer[2], 1))
                return;
            answer_size = answer[2];
            answer_data = new unsigned char[answer_size];
            take_byte_count = 1;

        } else if ((answer[1] == 0x05) || (answer[1] == 0x06) || (answer[1] == 0x0B) ||
                   (answer[1] == 0x0F) || (answer[1] == 0x10)) {

            //qDebug() << "received function 2";
            answer_data = new unsigned char[5];
            answer_size = 4;
            take_byte_count = 0;
        } else if (answer[1] == 0x07) {
            //qDebug() << "Modbus: received function 7";
            answer_data = new unsigned char[2];
            answer_size = 1;
            take_byte_count = 0;
        } else if (answer[1] == 0x08) {
            //qDebug() << "Modbus: received function 8";
            answer_data = new unsigned char[5];
            answer_size = 4;
        } else if (answer[1] == 0x16) {
            //qDebug() << "Modbus: received function 16";
            answer_data = new unsigned char[7];
            answer_size = 6;
            take_byte_count = 0;
        } else if ((answer[1] == 0x18)) { // FIFO
            //qDebug() << "Modbus: received function 18";
            if (!tryRead((unsigned char*) &answer_size, 2))
                return;
            answer_data = new unsigned char[answer_size + 1];
            take_byte_count = 2;
        }

        // read answer_data
        if (!tryRead(answer_data, answer_size)) {
            delete[] answer_data;
            return;
        }

        /*  // Debug!
        qDebug() << "answer size: " << answer_size;
        qDebug() << "answer: ";
        for (int z=0; z<answer_size; z++){
            qDebug() << answer_data[z] << z;
        }
        */ // /Debug!

        if (answer[1] == 0x08) { //Modbus test function
            if ((answer_data[1] == '\x00') && ((answer_data[2] != '\x54') || (answer_data[3] != '\x65'))) {
                qDebug() << "Modbus: Function 0x08 with subfunction 0000 returned and failed!";
                qDebug() << "Modbus: Wrong data returned by sensor";
            } else {
                qDebug() << "Modbus: Function 0x08 with subfunction 00" << answer_data[1] << " returned! ";
            }
        }
        //CRC part
        //qDebug() << "Modbus: reading CRC ...";
        if (!tryRead((unsigned char*) &sent_crc, sizeof(short))) {
            delete[] answer_data;
            return;
        }
        if(checkResponseCRC(answer, answer_data, answer_size, take_byte_count, sent_crc)) {
            if (answer[1] != 0x08) { //if not modbus test function
                //ret.append(answer[0]).append(answer[1]);
                QString numb;
                QString newKey;
                QString newVal;
                newKey.append("FromDevice").append(numb.setNum(answer[0], 16)).append("WithFunc").append(numb.setNum(answer[1], 16));
                //newVal.append(numb.setNum(answer_data[0]));
                for (int i = 0; i < answer_size; i++) {
                    //newVal.append(numb.setNum(answer_data[i]));
                    newVal.append("|").append(numb.setNum(answer_data[i], 16));
                }
                //qDebug() << newVal;
                //Message mesg(ret, QString::fromAscii((char *) answer_data, answer_size));
                MessageSample mesg(newKey, newVal);
                msgQue.push_back(mesg);
            }
            //qDebug() << "CRC of received message is ok!";
        } else
            qDebug() << "Modbus: CRC of received data is wrong!";
        delete []answer_data;
        // /CRC part

    } else { // error arrived
        if (!tryRead(&answer[2], 1))
            return;
        if (!tryRead((unsigned char*) &sent_crc, sizeof(short)))
            return;
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
    emit readyToRead();
    //qDebug() << "Modbus: Read ends";
}

int ModbusStudent::checkResponseCRC(unsigned char* answer, unsigned char* answer_data,
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


ModbusStudent::~ModbusStudent(){
        if ((close(fd)) < 0)
            qDebug() << "Couldn't close file descriptor";
        delete portNotifier;
}
