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

    virtual void connect();

public slots:
    virtual void send(Message &payload);

signals:
    void received(Message &payload);

private:
    QTimer timer;
    QQueue<Message> toSend;
    QQueue<Message> receivedMessages;

private slots:
    //
    // emits received signals
    //
    void sendAndReceiveData();

};

#endif // DUMMYSENSOR_H
