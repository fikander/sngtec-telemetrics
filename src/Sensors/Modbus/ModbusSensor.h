#ifndef MODBUS_H
#define MODBUS_H

#include <QObject>
#include <QSocketNotifier>

#include "Sensors/Sensor.h"
#include "KeyValueMap.h"

#include "modbus.h"

class Modbus : public Sensor
{

    Q_OBJECT

public:
    Modbus(KeyValueMap &config);
    virtual ~Modbus();

    virtual int connect();

public slots:
    // send to the device
    virtual void send(QSharedPointer<Message> payload);

signals:
    // emitted for every sample/event received from the device
    void received(QSharedPointer<Message> payload);

private slots:
      void pollModbus();

private:
    void modbusDisconnect();
    QByteArray modbusReadData(int slave, int functionCode, int startAddress, int noOfItems);

    QString portName;
    bool parityEven;
    int bandwidth;
    int timeout;
    bool modbusDebug;
    int modbusSlave;

    modbus_t *m_modbus;
};

#endif // MODBUS_H
