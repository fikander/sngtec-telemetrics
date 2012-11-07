#ifndef DEVCONNECTION_H
#define DEVCONNECTION_H

#include <QObject>
#include <QVector>
#include "Configurator/configurator.h"
#include "Message/message.h"

class DevConnection;
class MessageSample;

//Iterface for various sensors
class DevConnection : public QObject
{
    Q_OBJECT

public:
    //DevConnection* create(Configurator *config, int no); // = 0;

    virtual DevConnection* create(Configurator *config, int no) = 0; // = 0;
    virtual DevConnection* clone(Configurator *config, int no) = 0;
    virtual ~DevConnection() = 0;

    virtual void write(QVector<MessageSample>) = 0;
    virtual QVector<MessageSample> readAll() = 0;

signals:
    void readyToRead();
};

#endif // DEVCONNECTION_H
