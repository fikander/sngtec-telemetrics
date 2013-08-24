
#include "debug.h"
#include <qglobal.h>

#include "SNGConnectCloud.h"


SNGConnectCloud::SNGConnectCloud(KeyValueMap &config):
    Cloud()
{
    if (config.contains("interval")) {
        timer.setInterval(config["interval"].toUInt() * 1000);
    } else {
        timer.setInterval(60 * 1000);
    }

    // queue length that produces warning
    if (config.contains("stale_messages_warning_threshold")) {
        toSendMessagesWarningThreshold = config["stale_messages_warning_threshold"].toUInt();
    } else {
        toSendMessagesWarningThreshold = 25;
    }

    // queue of messages cannot become longer that this. oldest messages will be deleted
    if (config.contains("stale_messages_error_threshold")) {
        toSendMessagesErrorThreshold = config["stale_messages_error_threshold"].toUInt();
    } else {
        toSendMessagesErrorThreshold = 100;
    }

    // how many times a single message has to fail to be processed to be removed from queue
    if (config.contains("failed_message_error_threshold")) {
        toSendMessageFailureCountThreshold = config["failed_message_error_threshold"].toUInt();
    } else {
        toSendMessageFailureCountThreshold = 100;
    }

    api = QSharedPointer<SNGConnectAPI>(new SNGConnectAPI(config));

    semaphore1 = new QSemaphore(1);
    semaphore2 = new QSemaphore(1);
}


SNGConnectCloud::~SNGConnectCloud()
{
    api.clear();
    foreach (MessageProxy *proxy, toSend) {
        delete proxy;
    }
    toSend.clear();
    receivedMessages.clear();
    delete semaphore1;
    delete semaphore2;
}

void SNGConnectCloud::connect()
{
    timer.start();
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(sendAndReceiveData()));
    m_connected = true;
}

/**
 * @brief SNGConnectCloud::send
 * Called as slot by sensors to send message (sample, request etc.) to cloud.
 * This is the point where cloud collects messages to be processed. They're
 * copied and retained within Cloud object.
 * @param payload
 */
void SNGConnectCloud::send(QSharedPointer<Message> payload)
{
    // events or requests are considered more important than samples,
    // therefore put them in front
    if (payload->getType() == Message::MsgEvent ||
        payload->getType() == Message::MsgResponse)
    {
        toSend.prepend(new MessageProxy(payload));
        sendAndReceiveData();
    } else {
        toSend.enqueue(new MessageProxy(payload));
    }
}

/**
 * @brief SNGConnectCloud::cleanupProcessedMessages
 * Messages that are succesfully sent to the cloud (processed = true)
 * are cleaned up.
 * If too many messages are stale - truncate the queue.
 */
void SNGConnectCloud::cleanupProcessedMessages()
{
    QList<int> toRemove;

    int i = 0;
    foreach(MessageProxy *proxy, toSend) {
        // remove from the queue when processed or permanently failed
        if (proxy->isProcessed()) {
            toRemove.append(i);
        } else if (proxy->getProcessingFailed() > toSendMessageFailureCountThreshold) {
            QERROR << "Message failed to be processed >" <<
                toSendMessageFailureCountThreshold << " times, deleting! : "
                << proxy->toString();
            toRemove.append(i);
        }
        ++i;
    }

    int removed = 0;
    foreach(int idx, toRemove) {
        MessageProxy* proxy = toSend.takeAt(idx - removed++);
        delete proxy;
    }

    // too many: remove some old messages
    if (toSend.count() > toSendMessagesErrorThreshold) {
        int countToRemove = toSend.count() - toSendMessagesErrorThreshold;
        QERROR << "Too many messages in the queue, removing " <<
            countToRemove<< " oldest!";

        for (int i = 0; i < countToRemove; i++) {
            MessageProxy *proxy = toSend.takeFirst();
            delete proxy;
        }
    }

    // close to overfilling the queue - warn about stale messages
    if (toSend.count() > toSendMessagesWarningThreshold) {
        QWARNING << toSend.count() << " messages not sent yet!";
    } else {
        QDEBUG << "Cleaned up " << removed << " messages.";
    }
}

/**
 *
 */
void SNGConnectCloud::processResponseMessages(
    QList< MessageProxy* > &responses)
{
    foreach(MessageProxy* proxy, responses) {
        if ((*proxy)->getType() != Message::MsgResponse) {
            continue;
        }

        QSharedPointer<MessageResponse> response =
            proxy->object().staticCast<MessageResponse>();

        if (response->command == "upload_log") {
            APICallSendLog *call = new APICallSendLog(api, proxy);
            // note: call will get self destroy after execution
            call->invoke();
        }
    }
}

/**
 * @brief SNGConnectCloud::sendAndReceiveData
 * Main function called from time to time to issue requestes to the cloud -
 * send data, poll for updates etc.
 */
void SNGConnectCloud::sendAndReceiveData()
{
    // dont do anything if not connected or problems with connection.
    // timer will retry anyway
    if (!m_connected) {
        return;
    }

    // clean up messages that have been successfully sent
    cleanupProcessedMessages();

    //
    // send system response to the cloud first
    //
    QList< MessageProxy* > allResponses;
    MessageProxy::getUnlockedMessages(
        toSend, Message::MsgResponse, allResponses);
    processResponseMessages(allResponses);

    //
    // send data to the cloud: samples and events
    //
    QList< MessageProxy* > allEvents;
    MessageProxy::getUnlockedMessages(
        toSend, Message::MsgEvent, allEvents);

    //TODO : Send events in batch, just like datapoints for different datastreams
    foreach(MessageProxy *proxy, allEvents)
    {
        APICallSendEvent *call = new APICallSendEvent(api, proxy);
        // note: call will get self destroy after execution
        call->invoke();
    }

    QList< MessageProxy* > allSamples;
    MessageProxy::getUnlockedMessages(
        toSend, Message::MsgSample, allSamples);

    if (!allSamples.isEmpty()) {
        APICallSendDatastreamSamples *call =
            new APICallSendDatastreamSamples(api, allSamples);
        // note: call will get self destroyed after execution
        call->invoke();
    }

    //
    // receive new data, convert to messages
    //
    // note: semaphore is locked and unlocked by API function
    if (semaphore1->tryAcquire()) {
        APICallGetDataStreams *call =
            new APICallGetDataStreams(api, semaphore1, "requested", &receivedMessages);
        // note: call will get self destroy after execution
        call->invoke();
    }

    // receive new commands
    // note: semaphore is locked and unlocked by API function
    if (semaphore2->tryAcquire()) {
        APICallGetCommands *call2 =
            new APICallGetCommands(api, semaphore2, &receivedMessages);
        // note: call will get self destroy after execution
        call2->invoke();
    }

    // emit messages, so that connected sensors catch them
    // note, that about http calls work asynchronously,
    // so results won't be processed immediately
    while (!receivedMessages.isEmpty()) {
        QSharedPointer<Message> message = receivedMessages.dequeue();
        emit received(message);
        QDEBUG << "Message received from cloud and sent to sensors: " +
            message->toString();
    }

    QDEBUG << "Bytes sent:" << api->getBytesSent() <<
        "Bytes received: " << api->getBytesReceived();
}
