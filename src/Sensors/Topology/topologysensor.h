#ifndef TOPOLOGYSENSOR_H
#define TOPOLOGYSENSOR_H

#include "Sensors/devconnection.h"
#include <QTcpServer>
#include <QTcpSocket>

class TopologySensor : public DevConnection
{
    Q_OBJECT

public:
    virtual DevConnection* create(Configurator *, int);
    virtual DevConnection* clone(Configurator *, int);
    virtual ~TopologySensor();

    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    TopologySensor(Configurator*, int);
    TopologySensor();
signals:
    void readyToRead();

private slots:
    void handleConnection();
    void readData();
    void deleteConnection();
    void handleError(QAbstractSocket::SocketError);

private:
    QTcpServer *tcpServer;
    QTcpSocket *clientConnection;
    qint16 port;

    QVector<Message> msgQue;
};

#endif // TOPOLOGYSENSOR_H
