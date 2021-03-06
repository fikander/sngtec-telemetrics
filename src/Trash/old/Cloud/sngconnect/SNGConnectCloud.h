#ifndef SNGCONNECT_H
#define SNGCONNECT_H

#include <QTimer>
#include <QQueue>

#include "../Cloud.h"
#include "KeyValueMap.h"
#include "Message/message.h"
#include "SNGConnectAPI.h"

class SNGConnectCloud : public Cloud
{
    Q_OBJECT

public:
    SNGConnectCloud(KeyValueMap &config);
    virtual ~SNGConnectCloud();

    virtual void connect();

public slots:
    virtual void send(QSharedPointer<Message> payload);

signals:
    void received(QSharedPointer<Message> payload);

private:
    QTimer timer;
    QQueue< QSharedPointer<Message> > toSend;
    QQueue< QSharedPointer<Message> > receivedMessages; // dispatched as received signal (for sensors)

    QSharedPointer<SNGConnectAPI> api;

    // queue length that produces warning
    int toSendMessagesWarningThreshold;
    // queue of messages cannot become longer that this. oldest messages will be deleted
    int toSendMessagesErrorThreshold;
    // how many times a single message has to fail to be processed to be removed from queue
    int toSendMessageFailureCountThreshold;

    void cleanupProcessedMessages();

private slots:

    // emits received signals
    void sendAndReceiveData();

private:
    // using messages as poor man semaphores to avoid calling some APIs while others haven't finished
    QSharedPointer<Message> semaphore1, semaphore2;

    void processResponseMessages(QList< QSharedPointer<Message> > &responses);

};

#endif // SNGCONNECT_H
