#include "modbus.h"
#include <iostream>
#include <QByteArray>
#include <QVector>
#include <QDebug>
#include <errno.h>

Modbus* Modbus::create(Configurator *config){
    // TODO
    return new Modbus("/dev/ttyS0", "normal");
}

Modbus* Modbus::clone(Configurator *config){
    // TODO
    return new Modbus("/dev/ttyS0", "normal");
}

Modbus::Modbus(std::string port, std::string connection){
        preparePort(port);
        // Sends signal which sends signal to who?
        portNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
}


int Modbus::preparePort(std::string port){
        /* Check params! */
        if ((fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
                std::cout << "Open port failure" << fd << std::endl;
        termios* port_param = (termios*) malloc(sizeof(termios));
        port_param->c_cflag = B9600 | CS8;
        port_param->c_iflag = IGNPAR;
        port_param->c_oflag = 0;
        port_param->c_lflag = 0;
        port_param->c_cc[VMIN] = 1;
        port_param->c_cc[VTIME] = 0;

        //if (tcsetattr(fd, TCSAFLUSH, port_param) < 0)
              //  std::cout<< "Setting port attr error" << std::endl;
        free(port_param);
        return 0;
}

int Modbus::connect(unsigned char addr){
        sensor_addr = addr;
        return 0;
}

void Modbus::decodeMessage(Message msg){
        /* todo */
}

void Modbus::write( QVector<Message> messages){
        // queue messages?
        for (QVector<Message>::iterator it = messages.begin(); it < messages.end(); it++) {
                //decodeMessage(it*);
                struct ModbusRtuFrame* frame = (struct ModbusRtuFrame*) malloc(sizeof(unsigned char) * 10);
                //Fake data, official are uknnown
                // Test for funtion nr 4
                frame->addr = sensor_addr;
                frame->function = '4';
                frame->data = (unsigned char*) frame + 2;
                unsigned char c = (unsigned char) 'h';
                (frame->data[0]) = (unsigned char) c; //0x00; // register adr
                frame->data[1] = c;
                frame->data[2] = c;//0x01; // length of data
                frame->data[3] = c;
                frame->crc = (unsigned short int) qChecksum((char*) frame, 6);
                frame_length = 8;
                //Send
                int temp = 0;
                if ((temp = ::write(fd, frame, frame_length)) < 0)
                    qDebug() << "write error!" << errno;
                qDebug() << temp;
                free(frame);
        }
}


QVector<Message> Modbus::readAll(){
        struct ModbusRtuFrame* answer = (struct ModbusRtuFrame*)
                               malloc(frame_length * sizeof(unsigned char));
        // Read for function nr 0x04
        answer->data = (unsigned char*) malloc(4 * sizeof(unsigned char));
        int readed;
        if ((readed = read(fd, answer, frame_length)) < 0)
            qDebug() << "Read error!" << errno;
        qDebug() << readed;
        //przetworz
        unsigned char* odpowiedz = (unsigned char*) answer;
        Message* mesg = new Message();
        mesg->key = "Chciany klucz";
        mesg->value = QString(odpowiedz[4]);
        QVector<Message>* vector = new QVector<Message>();
        vector->push_front(*mesg);
        free(answer);
        return *vector;
}

Modbus::~Modbus(){
        if ((close(fd)) < 0)
                std::cout << "Couldn't close file descriptor" << std::endl;
}
