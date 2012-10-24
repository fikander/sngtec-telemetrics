
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

    if (config.contains("stale_messages_warning_threshold"))
        toSendMessagesWarningThreshold = config["stale_messages_warning_threshold"].toUInt();
    else
        toSendMessagesWarningThreshold = 25;

    if (config.contains("stale_messages_error_threshold"))
        toSendMessagesErrorThreshold = config["stale_messages_error_threshold"].toUInt();
    else
        toSendMessagesErrorThreshold = 100;

    api = QSharedPointer<SNGConnectAPI>(new SNGConnectAPI(config));
}

SNGConnectCloud::~SNGConnectCloud()
{
    api.clear();
    toSend.clear();
    receivedMessages.clear();
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

void SNGConnectCloud::cleanupProcessedMessages()
{
    QList<int> toRemove;

    int i = 0;
    foreach(QSharedPointer<Message> msg, toSend)
    {
        if (msg->isProcessed())
            toRemove.append(i);
        ++i;
    }

    int removed = 0;
    foreach(int idx, toRemove)
    {
        toSend.removeAt(idx - removed++);
    }

    // too many: remove some old messages
    if (toSend.count() > toSendMessagesErrorThreshold)
    {
        int countToRemove = toSend.count() - toSendMessagesErrorThreshold;
        QERROR << "Too many messages in the queue, removing " << countToRemove<< " oldest!";

        for (int i = 0; i < countToRemove; i++)
            toSend.removeFirst();
    }

    // close to overfilling the queue
    if (toSend.count() > toSendMessagesWarningThreshold)
    {
        QWARNING << toSend.count() << " messages not sent yet!";
    }
    else
    {
        QDEBUG << "Cleaned up " << removed << " messages.";
    }
}


void SNGConnectCloud::sendAndReceiveData()
{
    // dont do anything if not connected or problems with connection. timer will retry anyway
    if (!m_connected)
        return;

    // clean up messages that have been successfully sent
    cleanupProcessedMessages();

    //
    // send data
    //
    QList< QSharedPointer<Message> > allEvents;
    Message::getUnlockedMessages(toSend, Message::MsgEvent, true, allEvents);

    foreach(QSharedPointer<Message> event, allEvents)
    {
        APICallSendEvent *call = new APICallSendEvent(api, event);
        // note: call will get self destructed after execution
        call->invoke();
    }

    QList< QSharedPointer<Message> > allSamples;
    Message::getUnlockedMessages(toSend, Message::MsgSample, true, allSamples);

    while (!allSamples.isEmpty())
    {
        // get first sample to learn what datasource should be reported
        QSharedPointer<MessageSample> firstSample = allSamples[0].staticCast<MessageSample>();
        QList< QSharedPointer<MessageSample> > samples;
        MessageSample::takeMessagesByDatastream(allSamples, firstSample->key, samples);

        APICallSendDatastreamSamples *call = new APICallSendDatastreamSamples(api, firstSample->key, samples);
        // note: call will get self destructed after execution
        call->invoke();
    }

    //
    // TODO: receive new data, convert to messages
    //
    //for (int i = 0; i < qrand() % 10 + 1; i++)
    //    receivedMessages.enqueue( QSharedPointer<Message>(new MessageSample("fromCloud", "value")) );
    APICallGetDataStreams *call = new APICallGetDataStreams(api, "requested", &receivedMessages);
    call->invoke();

    // emit messages, so that connected sensors catch them
    while (!receivedMessages.isEmpty())
    {
        QSharedPointer<Message> message = receivedMessages.dequeue();
        emit received(message);
        QDEBUG << "Message received from cloud: " + message->toString();
    }
}

