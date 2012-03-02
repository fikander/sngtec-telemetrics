#ifndef MODBUSRTUFRAME_H
#define MODBUSRTUFRAME_H

#include <QByteArray>

class ModbusRtuFrame {

    public: // add change function?
        ModbusRtuFrame(unsigned char function, int new_data_length);
        void setAddr(unsigned char new_addr);
        void setData(unsigned char* new_data, short new_data_length);
        void setData(QByteArray new_data);
        void countCRC();
        short int showSize();
        unsigned char** toSend();

        void destroyFrame();
        unsigned char getFunction();
        unsigned char getAddr();
        unsigned char* getData();
        unsigned short int getCRC();

    private:
        unsigned char addr;
        unsigned char function;
        short data_length;
        unsigned char* data;
        unsigned short int crc;

};

#endif // MODBUSRTUFRAME_H
