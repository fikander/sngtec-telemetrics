#ifndef CLOCONNECTION_H
#define CLOCONNECTION_H

#include <QObject>
#include <QVector>
#include "Message/message.h"


class MessageSample;
class Configurator;

/*
 * Interface for various connection types with cloud
 */
class CloConnection : public QObject {

    Q_OBJECT

public:
    typedef QVector<MessageSample> MessagesSet;

    virtual CloConnection* create(Configurator *config) = 0;
    virtual CloConnection* clone(Configurator *config) = 0;
    virtual void connect() = 0;
    virtual ~CloConnection() = 0;
    virtual void write(QVector<MessageSample>) = 0;
    virtual QVector<MessageSample> readAll() = 0;
    virtual bool isBusy() = 0;

// Unused so far
signals:
    void readyToRead();
    void readyToWrite();
    void orderReceived(QVector<MessageSample>);
};

#endif // CLOCONNECTION_H
