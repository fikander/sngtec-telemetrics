#ifndef MODBUSRTUFRAME_H
#define MODBUSRTUFRAME_H

#include <QByteArray>

// libmodbus: http://www.libmodbus.org/documentation/
// http://www.ni.com/white-paper/7675/en
// Java opensource Modbus implementation http://jamod.sourceforge.net/
// http://jamod.sourceforge.net/kbase/protocol.html#implementations

// qmodbus - uses libmodbus (modified for polling!) and qextserialport
// - modbus.c has extra functions: modbus_poll, which calls busMonitorAddItem - this is defined in main Qt program as extern "C"

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
