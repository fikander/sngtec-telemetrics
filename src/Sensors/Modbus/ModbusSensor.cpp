
#include <errno.h>

#include "ModbusSensor.h"
#include "modbus.h"
#include "modbus-private.h"

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

    if (config.contains("timeout"))
        timeout = config["timeout"].toUInt();
    else
        timeout = 5;

    if (config.contains("modbusDebug"))
        modbusDebug = (config["modbusDebug"].toString().toLower() == "true");
    else
        modbusDebug = false;

    if(config.contains("modbusSlave"))
        modbusSlave = config["modbusSlave"].toUInt();
    else
        modbusSlave = 1;
}

Modbus::~Modbus()
{
    modbusDisconnect();
}

void Modbus::modbusDisconnect()
{
    if(m_modbus) {
        modbus_close(m_modbus);
        modbus_free(m_modbus);
        m_modbus = NULL;
    }

    m_connected = false;
}


int Modbus::connect()
{
    modbusDisconnect();

    char parity = parityEven ? 'E': 'O';

    m_modbus = modbus_new_rtu(portName.toAscii().constData(), bandwidth, parity, 8, 1);

    //Debug messages from libmodbus
    modbus_set_debug(m_modbus, modbusDebug);

    modbus_set_error_recovery(m_modbus, MODBUS_ERROR_RECOVERY_LINK);

    //Response timeout
    struct timeval response_timeout;
    response_timeout.tv_sec = timeout;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout(m_modbus, &response_timeout);

    if(m_modbus && modbus_connect(m_modbus) == -1) {
        QERROR << "Connection failed - could not connect to serial port " << portName;
        m_connected = false;
    }
    else
        m_connected = true;

    return 0;
}

void Modbus::send(QSharedPointer<Message> payload)
{

}

void Modbus::pollModbus()
{
    QByteArray result = modbusReadData(modbusSlave, _FC_READ_HOLDING_REGISTERS, 0, 1);

    emit received(QSharedPointer<Message>(new MessageSample("value", result.toHex())));
}

QByteArray Modbus::modbusReadData(int slave, int functionCode, int startAddress, int noOfItems)
{
    if (m_modbus == NULL) return QByteArray();
    if (!m_connected) return QByteArray();

    uint8_t dest[1024]; //setup memory for data
    uint16_t * dest16 = (uint16_t *) dest;
    memset(dest, 0, 1024);
    int ret = -1; //return value from read functions
    bool is16Bit = false;

    modbus_set_slave(m_modbus, slave);
    //request data from modbus
    switch(functionCode)
    {
            case _FC_READ_COILS:
                    ret = modbus_read_bits(m_modbus, startAddress, noOfItems, dest);
                    break;

            case _FC_READ_DISCRETE_INPUTS:
                    ret = modbus_read_input_bits(m_modbus, startAddress, noOfItems, dest);
                    break;

            case _FC_READ_HOLDING_REGISTERS:
                    ret = modbus_read_registers(m_modbus, startAddress, noOfItems, dest16);
                    is16Bit = true;
                    break;

            case _FC_READ_INPUT_REGISTERS:
                    ret = modbus_read_input_registers(m_modbus, startAddress, noOfItems, dest16);
                    is16Bit = true;
                    break;

            default:
                    break;
    }

    if(ret == noOfItems)
    {
        char *data = is16Bit ? (char*)dest16 : (char*)dest;
        return QByteArray(data, noOfItems * (is16Bit ? 2 : 1));
    }
    else
    {
        QString line;
        if(ret < 0) {
                line = QString("Slave threw exception  >  ").arg(ret) +  modbus_strerror(errno) + " ";
                QWARNING << line;

                // TODO: prepare event message with error

        }
        else {
                line = QString("Number of registers returned does not match number of registers requested!. [")  +  modbus_strerror(errno) + "]";
                QWARNING << line;
        }
     }

    return QByteArray();
}
