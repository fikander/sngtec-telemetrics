#include "modbusrtuframe.h"

#include <QByteArray>
#include <QDebug>
#include <string>


ModbusRtuFrame::ModbusRtuFrame(unsigned char new_function, int new_data_length){
    setAddr('1');
    function = new_function;
    data_length = new_data_length;
    data = new unsigned char[data_length];
    countCRC();
}

void ModbusRtuFrame::setAddr(unsigned char new_addr){
    addr = new_addr;
    countCRC();
}

void ModbusRtuFrame::setData(unsigned char* new_data, short new_data_length){
    if (new_data_length == data_length)
        memcpy(data, new_data, sizeof(data));
    else
        qDebug() << "Adding data error - wrong data size";
    countCRC();
}

short int ModbusRtuFrame::showSize(){
    return data_length;
}

void ModbusRtuFrame::countCRC(){
    unsigned char* tmp = new unsigned char[data_length + 2];
    tmp[0] = addr;
    tmp[1] = function;
    memcpy((tmp + 2), data, data_length);
    crc = qChecksum((char*) tmp, data_length + 2);
    //qDebug() << "CRC: " << crc;
    delete []tmp;
}

unsigned char** ModbusRtuFrame::toSend(){
    unsigned char** to_send = new unsigned char*[4];
    to_send[0] = &addr;
    to_send[1] = &function;
    to_send[2] = data;
    to_send[3] = (unsigned char*) &crc;
    return to_send;
}
