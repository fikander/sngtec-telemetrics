
#include "debug.h"
#include <qglobal.h>

#include "DummyCloud.h"


DummyCloud::DummyCloud(KeyValueMap &config):
    Cloud()
{
    timer.setInterval(config["interval"].toUInt() * 1000);
}

DummyCloud::~DummyCloud()
{

}

void DummyCloud::connect()
{
    timer.start();
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(sendAndReceiveData()));

    m_connected = true;
}

void DummyCloud::send(Message &payload)
{
    toSend.enqueue(payload);
}

void DummyCloud::sendAndReceiveData()
{
    // dont do anything if not connected or problems with connection. timer will retry anyway
    if (!m_connected)
        return;

    // send data
    QDEBUG << "Messages sent: " + QVariant(toSend.count()).toString();
    toSend.clear();

    // receive new data, convert to messages
    for (int i = 0; i < qrand() % 10 + 1; i++)
        receivedMessages.enqueue(Message());

    // emit messages, so that connected sensors catch them
    while (!receivedMessages.isEmpty())
    {
        Message message = receivedMessages.dequeue();
        emit received(message);
        QDEBUG << "Message received: " + message.toString();
    }
}

