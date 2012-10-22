
#include "debug.h"
#include <qglobal.h>

#include "SNGConnectCloud.h"


SNGConnectCloud::SNGConnectCloud(KeyValueMap &config):
    Cloud()
{
    if (config.contains("interval"))
        timer.setInterval(config["interval"].toUInt() * 1000);
    else
        timer.setInterval(60 * 1000);

    api = QSharedPointer<SNGConnectAPI>(new SNGConnectAPI(config));
}

SNGConnectCloud::~SNGConnectCloud()
{

}

void SNGConnectCloud::connect()
{
    // TODO: some authentication, getting tokens etc.
    timer.start();
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(sendAndReceiveData()));

    m_connected = true;
}

void SNGConnectCloud::send(QSharedPointer<Message> payload)
{
    toSend.enqueue(payload);
}


void SNGConnectCloud::sendAndReceiveData()
{
    // dont do anything if not connected or problems with connection. timer will retry anyway
    if (!m_connected)
        return;

    //
    // send data
    //
    QList< QSharedPointer<Message> > allSamples = Message::takeMessages(toSend, Message::MsgSample);
    while (!allSamples.isEmpty())
    {
        // get first sample to learn what datasource should be reported
        QSharedPointer<MessageSample> firstSample = allSamples[0].staticCast<MessageSample>();
        QList< QSharedPointer<MessageSample> > samples = MessageSample::takeMessagesByDatastream(allSamples, firstSample->key);

        // api call objects will get automatically deleted after invoke has been called
        APICallSendDatastreamSamples *call = new APICallSendDatastreamSamples(api, firstSample->key, samples);
        call->invoke();

        samples.clear();
    }

    QList< QSharedPointer<Message> > allEvents = Message::takeMessages(toSend, Message::MsgEvent);
    foreach(QSharedPointer<Message> event, allEvents)
    {
        APICallSendEvent *call = new APICallSendEvent(api, event);
        call->invoke();
    }

    Q_ASSERT(toSend.isEmpty());

    //
    // receive new data, convert to messages
    //
    //for (int i = 0; i < qrand() % 10 + 1; i++)
    //    receivedMessages.enqueue( QSharedPointer<Message>(new MessageSample("fromCloud", "value")) );

    // emit messages, so that connected sensors catch them
    while (!receivedMessages.isEmpty())
    {
        QSharedPointer<Message> message = receivedMessages.dequeue();
        emit received(message);
        QDEBUG << "Message received: " + message->toString();
    }
}

