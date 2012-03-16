#ifndef TOPOLOGYSENSOR_H
#define TOPOLOGYSENSOR_H

#include "Sensors/devconnection.h"

class TopologySensor : public DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator *config, int no) = 0;
    virtual DevConnection* clone(Configurator *config, int no) = 0;
    virtual ~DevConnection() = 0;

    virtual void write(QVector<Message>) = 0;
    virtual QVector<Message> readAll() = 0;

signals:
    void readyToRead();

};

#endif // TOPOLOGYSENSOR_H
