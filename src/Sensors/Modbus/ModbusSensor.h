#ifndef MODBUS_H
#define MODBUS_H

#include <QObject>
#include <QSocketNotifier>
#include <QTimer>
#include <QVector>

#include "Sensors/Sensor.h"
#include "KeyValueMap.h"

#include "modbus.h"

class Modbus : public Sensor
{

    Q_OBJECT

public:
    Modbus(KeyValueMap &config, QObject *parent = 0);
    virtual ~Modbus();

    virtual int connect();

public slots:
    // send to the device
    virtual void send(QSharedPointer<Message> payload);

signals:
    // emitted for every sample/event received from the device
    void received(QSharedPointer<Message> payload);

private slots:
      void sendAndReceiveData();

private:
    void modbusDisconnect();
    QVector<uint> modbusReadData(int functionCode, int startAddress, int noOfItems);
    bool modbusWriteData(int functionCode, int startAddress, QVector<uint> vector);

    QString portName;
    char parity;
    int bandwidth;
    int dataBits, stopBits;
    int timeout;
    bool modbusDebug;
    bool bigEndian;
    QTimer timer;

    modbus_t *m_modbus;

    class Query
    {
    public:
        Query();
        Query(QString name, int address, int count, bool bigEndian = true);
        QString name;
        int slave;
        int address;
        int count;
        bool bigEndian;
        QString eventType;
        char read_function, write_function;

        bool queried;
        QVector<uint> lastResult;

        QString toString()
        {
            return name + "[" + eventType +
                    "],slave:" + QString::number(slave) +
                    ",addr:" + QString::number(address) +
                    ",cnt:" + QString::number(count) +
                    ",read:0x" + QString::number(read_function, 16) +
                    ",write:0x" + QString::number(write_function, 16);
        }
    };
    QVector<Query> queries;
};

#endif // MODBUS_H
