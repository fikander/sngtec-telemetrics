#include "modbusrtuframe.h"
#include "Sensors/Modbus/crc-count.h"
#include <QByteArray>
#include <QDebug>
#include <string>


ModbusRtuFrame::ModbusRtuFrame(unsigned char new_function, int new_data_length){
    function = new_function;
    data_length = new_data_length;
    data = new unsigned char[data_length + 1];
    setAddr('1');
}

void ModbusRtuFrame::setAddr(unsigned char new_addr){
    addr = new_addr;
    countCRC();

}

void ModbusRtuFrame::setData(unsigned char* new_data, short new_data_length){
    if (new_data_length == data_length) {
        memcpy(data, new_data, data_length);
        data[data_length] = '\0';
    } else
        qDebug() << "Adding data error - wrong data size";
    countCRC();
}

void ModbusRtuFrame::setData(QByteArray new_data){
    if (new_data.size() == data_length) {
        memcpy(data, new_data.data(), new_data.size());
    } else {
        qDebug() << "Adding data error - wrong data size?" << data_length << new_data.size();
    }
}

short int ModbusRtuFrame::showSize(){
    return data_length;
}

void ModbusRtuFrame::countCRC(){
    unsigned char* tmp = new unsigned char[data_length + 3];
    tmp[0] = addr;
    tmp[1] = function;
    memcpy(&tmp[2], data, data_length);
    tmp[data_length + 2] = '\0';
    crc = modbusCRC(tmp, data_length + 2);
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

void ModbusRtuFrame::destroyFrame(){ // to powinien być destruktor!
    delete []data;
}

unsigned char ModbusRtuFrame::getFunction(){
    return function;
}

unsigned char ModbusRtuFrame::getAddr(){
    return addr;
}

unsigned char* ModbusRtuFrame::getData(){
    return data;
}

unsigned short int ModbusRtuFrame::getCRC(){
    return crc;
}
