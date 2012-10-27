
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

/**
 * @brief SNGConnectCloud::send
 * Called as slot by sensors to send message (sample, request etc.) to cloud
 * @param payload
 */
void SNGConnectCloud::send(QSharedPointer<Message> payload)
{
    // events or requests are considered more important than samples, therefore put them in front
    if (payload->getType() == Message::MsgEvent ||
        payload->getType() == Message::MsgResponse)
    {
        toSend.prepend(payload);
        sendAndReceiveData();
    }
    else
        toSend.enqueue(payload);
}

/**
 * @brief SNGConnectCloud::cleanupProcessedMessages
 * Messages that are succesfully sent to the cloud (processed = true) are cleaned up.
 * If too many messages are stale - truncate the queue.
 */
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

    // close to overfilling the queue - warn about stale messages
    if (toSend.count() > toSendMessagesWarningThreshold)
    {
        QWARNING << toSend.count() << " messages not sent yet!";
    }
    else
    {
        QDEBUG << "Cleaned up " << removed << " messages.";
    }
}

/**
 * @brief SNGConnectCloud::sendAndReceiveData
 * Main function called from time to time to issue requestes to the cloud - send data, poll for updates etc.
 */
void SNGConnectCloud::sendAndReceiveData()
{
    // dont do anything if not connected or problems with connection. timer will retry anyway
    if (!m_connected)
        return;

    // clean up messages that have been successfully sent
    cleanupProcessedMessages();

    //
    // send system response to the cloud first
    //
    QList< QSharedPointer<Message> > allResponses;
    Message::getUnlockedMessages(toSend, Message::MsgResponse, true, allResponses);

    // TODO: send responses, unlock them after sending

    //
    // send data to the cloud: samples and events
    //

    QList< QSharedPointer<Message> > allEvents;
    Message::getUnlockedMessages(toSend, Message::MsgEvent, true, allEvents);

    foreach(QSharedPointer<Message> event, allEvents)
    {
        APICallSendEvent *call = new APICallSendEvent(api, event);
        // note: call will get self destroy after execution
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
        // note: call will get self destroy after execution
        call->invoke();
    }

    //
    // receive new data, convert to messages
    //
    APICallGetDataStreams *call = new APICallGetDataStreams(api, "requested", &receivedMessages);
    // note: call will get self destroy after execution
    call->invoke();

    // receive new commands
    APICallGetCommands *call2 = new APICallGetCommands(api, &receivedMessages);
    call2->invoke();

    // emit messages, so that connected sensors catch them
    // note, that about http calls work asynchronously, so results won't be processed immediately
    while (!receivedMessages.isEmpty())
    {
        QSharedPointer<Message> message = receivedMessages.dequeue();
        emit received(message);
        QDEBUG << "Message received from cloud and sent to sensors: " + message->toString();
    }

}

