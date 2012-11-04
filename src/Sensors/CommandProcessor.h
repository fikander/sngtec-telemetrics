#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "Sensors/Sensor.h"
#include "Message/message.h"
#include "KeyValueMap.h"

class CommandProcessor : public Sensor
{
    Q_OBJECT
public:
    CommandProcessor(KeyValueMap &config, QObject *parent = 0);
    virtual ~CommandProcessor();

    virtual int connect();

public slots:
    virtual void send(QSharedPointer<Message> payload);

signals:
    void received(QSharedPointer<Message> payload);

};

#endif // COMMANDPROCESSOR_H
