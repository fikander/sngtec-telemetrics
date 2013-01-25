
#include <errno.h>

#include "ModbusSensor.h"
#include "modbus.h"
#include "modbus-private.h"

#include "debug.h"

#include "convertersfactory.h"

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

    const char ByteTimeout[] = "byte_timeout";
    const int DisabledByteTimeout = -1000;
    if (config.contains(ByteTimeout)) {
        bool ok;
        byte_timeout_ms = 1000 * config[ByteTimeout].toInt(&ok);
        if (!ok)
            byte_timeout_ms = DisabledByteTimeout;
    }
    else
        byte_timeout_ms = DisabledByteTimeout;

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

            //read converter parameters
            QString converterType = parameterValue.section(':', 5, 5);
            QString converterParams = parameterValue.section(':', 6, 6);
            bool isNone(false);
            q.converter = ConvertersFactory::create(converterType, converterParams, &isNone);
            if (0 == q.converter) {
                //set default converter
                if (!Query::converters()->contains(q.count) || !isNone) {
                    QWARNING << q.name << "Converter params:" << converterType << converterParams << "are incorrect and cannot set the default converter";
                }
                else {
                    q.converter = (*Query::converters())[q.count];
                }
            }

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

QHash<int, Converter*> *Modbus::Query::_converters = 0;
QHash<int, Converter *> *Modbus::Query::converters()
{
    if (0 == _converters) {
        _converters = new QHash<int, Converter*>();

        //create default converter for 1-word registers
        _converters->insert(1, ConvertersFactory::create("linear", "uint16_uint16_1_0"));
        //create default converter for 2-word registers
        _converters->insert(2, ConvertersFactory::create("linear", "uint32_uint32_1_0"));
    }
    return _converters;
}

Modbus::Query::Query():
    address(0), count(0), bigEndian(true), read_function(0), write_function(0), queried(false), converter(0)
{
}

Modbus::Query::~Query()
{
    //delete converter only if it's not shared among the others
//    if (!Query::static_converters.values().contains(converter))
//        delete converter;
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

    //set byte response timeout
    struct timeval response_byte_timeout;
    response_byte_timeout.tv_sec = byte_timeout_ms / 1000;
    response_byte_timeout.tv_usec = 1000 * (1000 * byte_timeout_ms - response_byte_timeout.tv_sec);
    modbus_set_byte_timeout(m_modbus, &response_byte_timeout);

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
            QVector<quint16> vector = q.revConvert(sample->value);

            //send it to modbus
            modbus_set_slave(m_modbus, q.slave);
            if (!modbusWriteData(q.write_function, q.address, vector)) {
                QWARNING << "Couldn't write" << sample->value << "for key" << sample->key;
            }

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
        QVector<quint16> result = modbusReadData(q.read_function, q.address, q.count);

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
            QString value = q.convert(result);

            QDEBUG << "NEW:" << q.toString() << ": " << value;

            // emit new message
            if (q.eventType == "readonly" || q.eventType == "readwrite")
            {
                emit received(QSharedPointer<Message>(new MessageSample(q.name, value)));
            }
            else if (q.eventType == "alarm")
            {
                int alarmValue = value.toInt();
                if (alarmValue > 0)
                    emit received(QSharedPointer<Message>(new MessageEvent(q.name, "alarm_on", alarmValue)));
                else
                    emit received(QSharedPointer<Message>(new MessageEvent(q.name, "alarm_off", alarmValue)));
            } else
            {
                // "system_error", "system_warning"
                int errWarValue = value.toInt();
                emit received(QSharedPointer<Message>(new MessageEvent(q.name, q.eventType, errWarValue)));
            }
        }
    }
}

bool Modbus::modbusWriteData(int functionCode, int startAddress, QVector<quint16> vector)
{
    if (m_modbus == NULL) return false;
    if (!m_connected) return false;

    const int BuffSize = 1024;
    uint8_t src[BuffSize]; //setup memory for data
    uint16_t * src16 = (uint16_t *) src;
    memset(src, 0, BuffSize);
    int ret = -1; //return value from write functions

    const bool is8bit = (_FC_WRITE_MULTIPLE_COILS == functionCode);//_FC_WRITE_SINGLE_COIL not included, since modbus_write_bit expects int

    const int BuffElementsCount = is8bit ? BuffSize : BuffSize / 2;

    //we make sure we won't write behind the buffer
    const int elementsCount = (vector.count() <= BuffElementsCount) ? vector.count() : BuffElementsCount;

    for (int i = 0; i < elementsCount; i++)
    {
        if (is8bit)
            src[i] = vector[i];
        else
            src16[i] = vector[i];
    }

    switch (functionCode) {
    case (_FC_WRITE_SINGLE_COIL): {
        //int modbus_write_bit(modbus_t *ctx, int coil_addr, int status);
        ret = modbus_write_bit(m_modbus, startAddress, src16[0]);
        if (1 != elementsCount)
            QDEBUG << "Not all the data bits got written to the coils (expected:" << elementsCount << ", written 1)";
        break;
    }
    case (_FC_WRITE_SINGLE_REGISTER): {
        //int modbus_write_register(modbus_t *ctx, int reg_addr, int value);
        //! \note the value is cast to int, what about the negative ones? Is this a server task to convert it accordingly?
        ret = modbus_write_register(m_modbus, startAddress, src16[0]);
        if (1 != elementsCount)
            QDEBUG << "Not all the data got written to the register (expected:" << elementsCount << ", written 1)";
        break;
    }
    case (_FC_WRITE_MULTIPLE_COILS): {
        //int modbus_write_bits(modbus_t *ctx, int addr, int nb, const uint8_t *data);
        ret = modbus_write_bits(m_modbus, startAddress, elementsCount, src);
        break;
    }
    case (_FC_WRITE_MULTIPLE_REGISTERS): {
        //int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *data);
        ret = modbus_write_registers(m_modbus, startAddress, elementsCount, src16);
        break;
    }
    default:
        break;
    }

    return (elementsCount == ret);
}

QVector<quint16> Modbus::modbusReadData(int functionCode, int startAddress, int noOfItems)
{
    if (m_modbus == NULL) return QVector<quint16>();
    if (!m_connected) return QVector<quint16>();

    uint8_t dest[1024]; //setup memory for data
    uint16_t * dest16 = (uint16_t *) dest;
    memset(dest, 0, 1024);
    int ret = -1; //return value from read functions
    bool is16Bit = false;

    /** \note : shouldn't we check for the num of items?
      const int BuffSize = 1024;
      const int MultFactor = (_FC_READ_INPUT_REGISTERS == functionCode || _FC_READ_INPUT_REGISTERS == functionCode) ? 2 : 1;
      const int NoOfBytesNeeded = MultFactor * noOfItems;
      if (NoOfBytesNeeded > BuffSize) {
        noOfItems = BuffSize / MultFactor;
      }
      */

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
        QVector<quint16> result;
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

    return QVector<quint16>();
}
