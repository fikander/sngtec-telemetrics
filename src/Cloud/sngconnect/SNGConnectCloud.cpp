
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

    semaphore1 = QSharedPointer<Message>(
        new Message(QDateTime::currentDateTime())
    );
    semaphore2 = QSharedPointer<Message>(
        new Message(QDateTime::currentDateTime())
    );
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
 * Called as slot by sensors to send message (sample, request etc.) to cloud.
 * This is the point where cloud collects messages to be processed. They're
 * copied and retained within Cloud object.
 * @param payload
 */
void SNGConnectCloud::send(QSharedPointer<Message> payload)
{
    QSharedPointer<Message> payloadCopy =
        QSharedPointer<Message>(payload->clone());

    // events or requests are considered more important than samples,
    // therefore put them in front
    if (payload->getType() == Message::MsgEvent ||
        payload->getType() == Message::MsgResponse)
    {
        toSend.prepend(payloadCopy);
        sendAndReceiveData();
    } else {
        toSend.enqueue(payloadCopy);
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
    foreach(QSharedPointer<Message> msg, toSend) {
        // remove from the queue when processed or permanently failed
        if (msg->isProcessed()) {
            toRemove.append(i);
        } else if (msg->getProcessingFailed() > toSendMessageFailureCountThreshold) {
            QERROR << "Message failed to be processed >" <<
                toSendMessageFailureCountThreshold << " times, deleting! : "
                << msg->toString();
            toRemove.append(i);
        }
        ++i;
    }

    int removed = 0;
    foreach(int idx, toRemove) {
        toSend.removeAt(idx - removed++);
    }

    // too many: remove some old messages
    if (toSend.count() > toSendMessagesErrorThreshold) {
        int countToRemove = toSend.count() - toSendMessagesErrorThreshold;
        QERROR << "Too many messages in the queue, removing " <<
            countToRemove<< " oldest!";

        for (int i = 0; i < countToRemove; i++)
            toSend.removeFirst();
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
    QList< QSharedPointer<Message> > &responses)
{
    foreach(QSharedPointer<Message> m, responses) {
        if (m->getType() != Message::MsgResponse) {
            continue;
        }

        QSharedPointer<MessageResponse> response = m.staticCast<MessageResponse>();

        if (response->command == "upload_log") {
            APICallSendLog *call = new APICallSendLog(api, response);
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
    QList< QSharedPointer<Message> > allResponses;
    Message::getUnlockedMessages(toSend, Message::MsgResponse, true, allResponses);
    processResponseMessages(allResponses);

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

    if (!allSamples.isEmpty()) {
        APICallSendMultipleDatastreamSamples *call =
            new APICallSendMultipleDatastreamSamples(api, allSamples);
        // note: call will get self destroy after execution
        call->invoke();
    }

    //
    // receive new data, convert to messages
    //
    // note: semaphore is locked and unlocked by API function
    if (!semaphore1->isLocked()) {
        APICallGetDataStreams *call =
            new APICallGetDataStreams(api, semaphore1, "requested", &receivedMessages);
        // note: call will get self destroy after execution
        call->invoke();
    }

    // receive new commands
    // note: semaphore is locked and unlocked by API function
    if (!semaphore2->isLocked()) {
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

    QDEBUG << "Bytes sent:" << api.getBytesSent() <<
        << "Bytes received: " << api.bytesReceived();
}
