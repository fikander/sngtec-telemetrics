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
    QQueue< QSharedPointer<Message> > receivedMessages;

    QSharedPointer<SNGConnectAPI> api;

    int toSendMessagesWarningThreshold;

    void cleanupProcessedMessages();

private slots:
    //
    // emits received signals
    //
    void sendAndReceiveData();

};

#endif // SNGCONNECT_H
