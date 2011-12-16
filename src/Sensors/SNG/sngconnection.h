#ifndef SNGCONNECTION_H
#define SNGCONNECTION_H

#include "src/Sensors/devconnection.h"
#include <QTcpSocket>

class SNGconnection : DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator &config);
    virtual DevConnection* clone(Configurator &config);
    virtual ~SNGconnection();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();

signals:
    void readyToRead();

public slots:
    void readFromSensor();
    void handleError(QAbstractSocket::SocketError);

private:
    SNGconnection(Configurator&);
    Configurator* conf;

    QString commServerHostName ;
    qint16 port;
    QTcpSocket* commServer;
};

#endif // SNGCONNECTION_H
