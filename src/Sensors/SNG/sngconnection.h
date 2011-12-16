#ifndef SNGCONNECTION_H
#define SNGCONNECTION_H

#include "src/Sensors/devconnection.h"

class SNGconnection : DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator &config);
    virtual DevConnection* clone(Configurator &config);
    virtual ~SNGconnection();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

private:
    SNGconnection(Configurator&);
    Configurator* conf;

signals:
    void readyToRead();
};

#endif // SNGCONNECTION_H
