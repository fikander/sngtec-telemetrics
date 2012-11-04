#ifndef MODBUS_H
#define MODBUS_H

#include <QObject>
#include <QSocketNotifier>

#include "Sensors/Sensor.h"
#include "KeyValueMap.h"

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
      void readFromSocket(int socket);

private:
    QString portName;
    bool parityEven;
    int bandwidth;
    int fd;

    QSocketNotifier *socketNotifier;

    int tryRead(unsigned char* buffer, int size);
    int checkResponseCRC(unsigned char* answer, unsigned char* answer_data,
                                 int answer_size, int take_byte_count,unsigned short crc);

};

#endif // MODBUS_H
