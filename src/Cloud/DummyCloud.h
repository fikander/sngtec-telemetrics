#ifndef MOCKCLOUD_H
#define MOCKCLOUD_H

#include <QTimer>
#include <QQueue>

#include "Cloud.h"
#include "Message/message.h"

class DummyCloud : public Cloud
{
    Q_OBJECT

public:
    DummyCloud(KeyValueMap &config);
    virtual ~DummyCloud();

    virtual void connect();

public slots:
    virtual void send(QSharedPointer<Message> payload);

signals:
    void received(QSharedPointer<Message> payload);

private:
    QTimer timer;
    QQueue< QSharedPointer<Message> > toSend;
    QQueue< QSharedPointer<Message> > receivedMessages;

private slots:
    //
    // emits received signals
    //
    void sendAndReceiveData();

};

#endif // MOCKCLOUD_H
