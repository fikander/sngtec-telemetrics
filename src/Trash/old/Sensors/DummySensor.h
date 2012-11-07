#ifndef DUMMYSENSOR_H
#define DUMMYSENSOR_H

#include <QTimer>
#include <QQueue>

#include "Sensor.h"
#include "KeyValueMap.h"
#include "Message/message.h"

class DummySensor : public Sensor
{

    Q_OBJECT

public:
    DummySensor(KeyValueMap &config);
    virtual ~DummySensor();

    virtual int connect();

public slots:
    // send to the device
    virtual void send(QSharedPointer<Message> payload);

signals:
    // emitted for every sample/event received from the device
    void received(QSharedPointer<Message> payload);

private:
    QTimer timer;
    QTimer dataGeneratorTimer;

    QQueue< QSharedPointer<Message> > toSend;
    QQueue< QSharedPointer<Message> > receivedMessages;

    QString dummyDataStream;

    int counter;

private slots:
    // emits received signals
    void sendAndReceiveData();

    // generate new sample
    void collectSample();

};

#endif // DUMMYSENSOR_H
