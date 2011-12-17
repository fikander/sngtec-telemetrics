#ifndef MOCKDEV_H
#define MOCKDEV_H

#include "src/Sensors/devconnection.h"

class MockDev : DevConnection
{
Q_OBJECT

public:
    virtual DevConnection* create(Configurator *config);
    virtual DevConnection* clone(Configurator *config);
    virtual ~MockDev();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

signals:
    void readyToRead();

};

#endif // MOCKDEV_H
