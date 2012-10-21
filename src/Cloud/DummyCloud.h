#ifndef MOCKCLOUD_H
#define MOCKCLOUD_H

#include <QTimer>
#include <QQueue>

#include "Cloud.h"
#include "KeyValueMap.h"
#include "Message/message.h"

class DummyCloud : public Cloud
{
    Q_OBJECT

public:
    DummyCloud(KeyValueMap &config);
    virtual ~DummyCloud();

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

#endif // MOCKCLOUD_H
