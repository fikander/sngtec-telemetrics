
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
        modbusDebug = config["modbusDebug"].toString().toLower() == "true";
    else
        modbusDebug = false;

    if (config.contains("modbusEndianness"))
        bigEndian = config["modbusEndianness"].toString().toLower() == "big";
    else
        bigEndian = true;

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

            q.slave = parameterValue.section(':', 2, 2).toInt();
            q.address = parameterValue.section(':', 3, 3).toInt();
            q.count = parameterValue.section(':', 4, 4).toInt();
            q.bigEndian = bigEndian;

            // [readonly|readwrite|alarm]
            q.eventType = parameterValue.section(':', 0, 0);

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
    name(name), address(address), count(count), bigEndian(bigEndian), read_function(0), write_function(0), queried(false)
{
}

Modbus::Query::Query():
    address(0), count(0), bigEndian(true), read_function(0), write_function(0), queried(false)
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
    if (payload->getType() == Message::MsgSample)
    {
        QSharedPointer<MessageSample> sample = payload.staticCast<MessageSample>();
        Query q;
        bool found = false;
        for (int i=0; i<queries.count(); i++)
        {
            if (queries.at(i).name == sample->key)
            {
                q = queries.at(i);
                found = true;
                break;
            }
        }

        if (found)
        {
            // convert string to vector of uint
            QVector<uint> vector;
            ulong value = sample->value.toLong();
            switch(q.count)
            {
            case 2:
                if (q.bigEndian)
                {
                    vector.append((value >> 8) & 0xFFFF); // high
                    vector.append(value & 0xFFFF); // low
                }
                else
                {
                    vector.append(value & 0xFFFF); // low
                    vector.append((value >> 8) & 0xFFFF); // high
                }
                break;
            case 1:
            default:
                vector.append(value & 0xFFFF);
            }

            //send it to modbus
            modbus_set_slave(m_modbus, q.slave);
            modbusWriteData(q.read_function, q.address, vector);

        }
        else
        {
            QWARNING << "Unknown messsage key: " << payload->toString();
        }

    }
    else
    {
        QWARNING << "Cannot handle this message: " << payload->toString();
    }
}

void Modbus::sendAndReceiveData()
{
    for (int i= 0; i < queries.count(); i++)
    {
        Query &q = queries[i];

        // query constants only once at the beginning
        if (q.eventType == "constant")
        {
            if (q.queried)
                continue;
            else
                q.queried = true;
        }

        modbus_set_slave(m_modbus, q.slave);
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
            if (q.eventType == "readonly" || q.eventType == "readwrite")
            {
                emit received(QSharedPointer<Message>(new MessageSample(q.name, QString::number(value))));
            }
            else if (q.eventType == "alarm")
            {
                if (value > 0)
                    emit received(QSharedPointer<Message>(new MessageEvent(q.name, "alarm_on", value)));
                else
                    emit received(QSharedPointer<Message>(new MessageEvent(q.name, "alarm_off", value)));
            } else
            {
                // "system_error", "system_warning"
                emit received(QSharedPointer<Message>(new MessageEvent(q.name, q.eventType, value)));
            }
        }
    }
}

bool Modbus::modbusWriteData(int functionCode, int startAddress, QVector<uint> vector)
{
    if (m_modbus == NULL) return false;
    if (!m_connected) return false;

    // TODO
    return false;
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
