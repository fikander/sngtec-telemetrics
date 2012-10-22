
#include "debug.h"
#include <qglobal.h>

#include "DummySensor.h"


DummySensor::DummySensor(KeyValueMap &config):
    Sensor()
{
    if (config.contains("interval"))
        timer.setInterval(config["interval"].toUInt() * 1000);
    else
        timer.setInterval(10 * 1000);

    if (config.contains("data_generator_interval"))
        dataGeneratorTimer.setInterval(config["data_generator_interval"].toUInt() * 1000);
    else
        dataGeneratorTimer.setInterval(5 * 1000);

    if (config.contains("dummy_datastream"))
        dummyDataStream = config["dummy_datastream"].toString();
    else
        dummyDataStream = "data_stream_1";
}

DummySensor::~DummySensor()
{

}

void DummySensor::connect()
{
    timer.start();
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(sendAndReceiveData()));

    dataGeneratorTimer.start();
    QObject::connect(&dataGeneratorTimer, SIGNAL(timeout()), this, SLOT(collectSample()));

    m_connected = true;
}

void DummySensor::send(QSharedPointer<Message> payload)
{
    toSend.enqueue(payload);
}

void DummySensor::collectSample()
{
    // just make one up...
    receivedMessages.enqueue(
            QSharedPointer<Message>(
                new MessageSample(dummyDataStream, QVariant(qrand() % 100).toString())
            ));
}

void DummySensor::sendAndReceiveData()
{
    // dont do anything if not connected or problems with connection. timer will retry anyway
    if (!m_connected)
        return;

    // send data
    QDEBUG << "Messages sent to the device: " + QVariant(toSend.count()).toString();
    toSend.clear();

    // emit messages (samples/events from the device), so that connected sensors catch them
    while (!receivedMessages.isEmpty())
    {
        QSharedPointer<Message> message = receivedMessages.dequeue();
        emit received(message);
        QDEBUG << "Message received: " + message->toString();
    }
}

