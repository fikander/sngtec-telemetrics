#include <termios.h>
#include <errno.h>
#include <fcntl.h>

#include "Modbus.h"
#include "crc-count.h"

#include "debug.h"

Modbus::Modbus(KeyValueMap &config)
{
    if (config.contains("bandwidth"))
        bandwidth = config["bandwidth"].toUInt();
    else
        bandwidth = 9600;

    if (config.contains("port"))
        portName = config["port"].toString();
    else
        portName = "/dev/ttyS1";

    if (config.contains("parity"))
        parityEven = (config["parity"].toString().toLower() == "even");
    else
        parityEven = false;

    socketNotifier = NULL;
    fd = -1;
}

Modbus::~Modbus()
{
    if (fd >= 0)
        close(fd);

    if (socketNotifier != NULL)
        delete socketNotifier;
}


int Modbus::connect()
{
    fd = open(portName.toAscii().data(), O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        QERROR << "Opening port " << fd << " failed (errno: " << errno << ")";
        return 1;
    }

    termios* port_param = new termios;
    port_param->c_cflag = CS8 | PARENB | CREAD | CLOCAL;
    if (bandwidth == 600)
        port_param->c_cflag = port_param->c_cflag | B600;
    else if (bandwidth == 1200)
        port_param->c_cflag = port_param->c_cflag | B1200;
    else if (bandwidth == 1800)
        port_param->c_cflag = port_param->c_cflag | B1800;
    else if (bandwidth == 2400)
        port_param->c_cflag = port_param->c_cflag | B2400;
    else if (bandwidth == 4800)
        port_param->c_cflag = port_param->c_cflag | B4800;
    else if (bandwidth == 9600)
        port_param->c_cflag = port_param->c_cflag | B9600;
    else if (bandwidth == 19200)
        port_param->c_cflag = port_param->c_cflag | B19200;
    else if (bandwidth == 38400)
        port_param->c_cflag = port_param->c_cflag | B38400;
    else if (bandwidth == 57600)
        port_param->c_cflag = port_param->c_cflag | B57600;
    else if (bandwidth == 115200)
        port_param->c_cflag = port_param->c_cflag | B115200;
    else {
        QWARNING << "Unknown bandwidth: "<< bandwidth << ", using default 9600";
        port_param->c_cflag = port_param->c_cflag | B9600;
    }

    if (!parityEven) //otherwise parity is even
        port_param->c_cflag = port_param->c_cflag | PARODD;

    port_param->c_iflag = IGNBRK | IGNPAR;
    port_param->c_oflag = 0;
    port_param->c_lflag = 0;
    port_param->c_cc[VMIN] = 1;
    port_param->c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSAFLUSH, port_param) < 0)
    {
        QERROR << "Error setting port attributes (errno: " << errno << ")";
        delete port_param;
        return 1;
    }

    delete port_param;

    if (socketNotifier)
        delete socketNotifier;

    socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);

    QObject::connect(socketNotifier, SIGNAL(activated(int)), this, SLOT(readFromSensor(int)), Qt::DirectConnection);

    return 0;
}


int Modbus::tryRead(unsigned char* buffer, int size)
{
    fd_set set;
    struct timeval timeout;
    int read_val;

    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    read_val = select(fd + 1, &set, NULL, NULL, &timeout);

    if (read_val == -1) {
        QDEBUG << "Read error (errno: " << errno << ")";
        return 1;
    } else if (read_val == 0) {
        QDEBUG << "Modbus read timeout";
        return 1;
    } else {
        read_val = read(fd, buffer, size);
        if (read_val < size) {
            if (read_val < 0)
                QDEBUG << "Read error (errno: " << errno << ")";
            else
                QDEBUG << "Answer too short";
            return 1;
        } else
            return 0;
    }
}

void Modbus::readFromSocket(int socket)
{
    unsigned char answer[3];
    unsigned char* answer_data = NULL;
    unsigned short crc;
    unsigned short sent_crc;
    short take_byte_count = 0; // whether answer[2] is active
    short answer_size = 0;

    if (tryRead(answer, 2))
        return;

    if (answer[1] < 0x80) { // normal function
        if (    (answer[1] == 0x01) || (answer[1] == 0x02) || (answer[1] == 0x03) ||
                (answer[1] == 0x04) || (answer[1] == 0x0C) || (answer[1] == 0x14) ||
                (answer[1] == 0x15) || (answer[1] == 0x17)) {

            if (tryRead(&answer[2], 1))
                return;
            answer_size = answer[2];
            answer_data = new unsigned char[answer_size];
            take_byte_count = 1;

        } else if ((answer[1] == 0x05) || (answer[1] == 0x06) || (answer[1] == 0x0B) ||
                   (answer[1] == 0x0F) || (answer[1] == 0x10)) {

            //qDebug() << "received function 2";
            answer_data = new unsigned char[5];
            answer_size = 4;
        } else if (answer[1] == 0x07) {
            //qDebug() << "Modbus: received function 7";
            answer_data = new unsigned char[2];
            answer_size = 1;
        } else if (answer[1] == 0x08) {
            //qDebug() << "Modbus: received function 8";
            answer_data = new unsigned char[5];
            answer_size = 4;
        } else if (answer[1] == 0x16) {
            //qDebug() << "Modbus: received function 16";
            answer_data = new unsigned char[7];
            answer_size = 6;
        } else if ((answer[1] == 0x18)) { // FIFO
            //qDebug() << "Modbus: received function 18";
            if (tryRead((unsigned char*) &answer_size, 2))
                return;
            answer_data = new unsigned char[answer_size + 1];
            take_byte_count = 2;
        }

        // read answer_data
        if (tryRead(answer_data, answer_size)) {
            delete[] answer_data;
            return;
        }


        if (answer[1] == 0x08) { //Modbus test function
            if ((answer_data[1] == '\x00') && ((answer_data[2] != '\x54') || (answer_data[3] != '\x65'))) {
                qDebug() << "Modbus: Function 0x08 with subfunction 0000 returned and failed!";
                qDebug() << "Modbus: Wrong data returned by sensor";
            } else {
                qDebug() << "Modbus: Function 0x08 with subfunction 00" << answer_data[1] << " returned! ";
            }
        }

        //read CRC
        if (tryRead((unsigned char*) &sent_crc, sizeof(short))) {
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

                received(QSharedPointer<Message>(new MessageSample(newKey, newVal)));
            }
        } else
            QDEBUG << "CRC of received data is wrong!";
        delete []answer_data;
        // /CRC part

    } else { // error arrived
        if (tryRead(&answer[2], 1))
            return;
        if (tryRead((unsigned char*) &sent_crc, sizeof(short)))
            return;
        crc = modbusCRC((unsigned char*) answer, 3);
        if (crc != sent_crc)
            QDEBUG << "CRC of received data is wrong!";
        else {
            QDEBUG << "Sensor send error (addr:" << answer[0] << ", error:" << answer[1] << ", exception: " << answer[2] << ")";
        }
    }
}


int Modbus::checkResponseCRC(unsigned char* answer, unsigned char* answer_data,
                             int answer_size, int take_byte_count,unsigned short crc)
{
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

void Modbus::send(QSharedPointer<Message> payload)
{

}
