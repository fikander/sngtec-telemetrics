
#include <errno.h>

#include "ModbusSensor.h"
#include "modbus.h"
#include "modbus-private.h"

#include "debug.h"

Modbus::Modbus(KeyValueMap &config, QObject *parent):
    Sensor(parent), m_modbus(NULL)
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
    {
        QString p = config["parity"].toString().toLower();
        if (p == "even")
            parity = 'E';
        else if (p == "odd")
            parity = 'O';
        else
            parity ='N';
    }
    else
        parity = 'N';

    if (config.contains("data_bits"))
        dataBits = config["data_bits"].toUInt();
    else
        dataBits = 8;

    if (config.contains("stop_bits"))
        stopBits = config["stop_bits"].toUInt();
    else
        stopBits = 1;

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

    if (config.contains("interval"))
        timer.setInterval(config["interval"].toUInt() * 1000);
    else
        timer.setInterval(60 * 1000);

    // find all parameters in the form:
    //    parameter_NAME=[readonly|readwrite|alarm]:[holding_register|input_register]:ADDRESS:COUNT
    foreach (QString key, config.keys())
    {
        if (key.startsWith("parameter_", Qt::CaseInsensitive))
        {
            Query q;

            q.name = key.mid(10);
            QString parameterValue = config[key].toString();

            q.address = parameterValue.section(':', 2, 2).toInt();
            q.count = parameterValue.section(':', 3, 3).toInt();

            // [readonly|readwrite|alarm]
            QString parameterMode = parameterValue.section(':', 0, 0);
            if (parameterMode == "alarm")
                q.eventType = "alarm";

            // [holding_register|input_register]
            QString parameterType = parameterValue.section(':', 1, 1);
            if (parameterType == "holding_register")
            {
                q.read_function = _FC_READ_HOLDING_REGISTERS;
                q.write_function = q.count > 1 ? _FC_WRITE_MULTIPLE_REGISTERS : _FC_WRITE_SINGLE_REGISTER;
            }
            else if (parameterType == "input_register")
            {
                q.read_function = _FC_READ_INPUT_REGISTERS;
                q.write_function = q.count > 1 ? _FC_WRITE_MULTIPLE_REGISTERS : _FC_WRITE_SINGLE_REGISTER;
            }
            else
                QWARNING << "Unknown parameter type: " << parameterType;

            queries.append(q);
        }
    }

    QDEBUG << "Found following parameters to track:";
    for (int i = 0; i < queries.count(); i++)
        QDEBUG << " - " << queries[i].toString();
}


Modbus::Query::Query(QString name, int address, int count, bool bigEndian):
    name(name), address(address), count(count), bigEndian(bigEndian), read_function(0), write_function(0)
{
}

Modbus::Query::Query():
    address(0), count(0), read_function(0), write_function(0)
{
}

Modbus::~Modbus()
{
    modbusDisconnect();
}

void Modbus::modbusDisconnect()
{
    timer.stop();

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

    m_modbus = modbus_new_rtu(portName.toAscii().constData(), bandwidth, parity, dataBits, stopBits);

    //Debug messages from libmodbus
    modbus_set_debug(m_modbus, modbusDebug);

    modbus_set_error_recovery(m_modbus, (modbus_error_recovery_mode)(MODBUS_ERROR_RECOVERY_LINK || MODBUS_ERROR_RECOVERY_PROTOCOL));

    //Response timeout
    struct timeval response_timeout;
    response_timeout.tv_sec = timeout;
    response_timeout.tv_usec = 0;
    modbus_set_response_timeout(m_modbus, &response_timeout);

    modbus_set_slave(m_modbus, modbusSlave);

    if(m_modbus && modbus_connect(m_modbus) == -1) {
        QERROR << "Connection failed - could not connect to serial port " << portName;
        m_connected = false;
        return 1;
    }
    else
        m_connected = true;

    timer.start();
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(sendAndReceiveData()));

    return 0;
}

void Modbus::send(QSharedPointer<Message> payload)
{

    //TODO
}

void Modbus::sendAndReceiveData()
{
    for (int i= 0; i < queries.count(); i++)
    {
        Query &q = queries[i];
        QVector<uint> result = modbusReadData(q.read_function, q.address, q.count);

        // dont create new messages if value hasnt changed from last sample
        if (q.lastResult == result)
        {
            QDEBUG << "OLD:" << q.toString();
            continue;
        }
        else
            q.lastResult = result;

        if (result.count() == q.count)
        {
            ulong value;
            // turn array of ints into string, based on endianness
            switch(q.count)
            {
            case 2:
                if (q.bigEndian)
                    value = result[0] * 0x10000 + result[1];
                else
                    value = result[1] * 0x10000 + result[0];
                break;
            case 1:
            default:
                value = result[0];
            }

            QDEBUG << "NEW:" << q.toString() << ": " << QString::number(value);

            // emit new message
            if (q.eventType == "alarm")
                emit received(QSharedPointer<Message>(new MessageEvent(q.name, "alarm_on", value)));
            else
                emit received(QSharedPointer<Message>(new MessageSample(q.name, QString::number(value))));
        }
    }
}

QVector<uint> Modbus::modbusReadData(int functionCode, int startAddress, int noOfItems)
{
    if (m_modbus == NULL) return QVector<uint>();
    if (!m_connected) return QVector<uint>();

    uint8_t dest[1024]; //setup memory for data
    uint16_t * dest16 = (uint16_t *) dest;
    memset(dest, 0, 1024);
    int ret = -1; //return value from read functions
    bool is16Bit = false;

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
        QVector<uint> result;
        QString string;
        for (int i = 0; i < noOfItems; i++)
        {
            result.append(is16Bit ? dest16[i] : dest[i]);
            string += is16Bit ? QString::number(dest16[i]) : QString::number(dest[i]);
        }
        //QDEBUG << "Modbus result: " << string;
        return result;
    }
    else
    {
        if(ret < 0) {
                QWARNING << "Slave threw exception  >  " << ret << modbus_strerror(errno);

                // TODO: prepare event message with error

        }
        else {
                QWARNING << "Number of registers returned does not match number of registers requested!. ["  << modbus_strerror(errno) << "]";
        }
     }

    return QVector<uint>();
}
