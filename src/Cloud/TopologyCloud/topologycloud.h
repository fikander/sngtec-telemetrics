#ifndef TOPOLOGYCLOUD_H
#define TOPOLOGYCLOUD_H

#include "Cloud/cloconnection.h"
#include <QTcpSocket>
#include "Message/message.h"


class Configurator;

class TopologyCloud : public CloConnection {
    Q_OBJECT
    
public:
    TopologyCloud(Configurator*);
    TopologyCloud(QString addr, int port);
    virtual CloConnection* create(Configurator*);
    virtual CloConnection* clone(Configurator*);
    virtual ~TopologyCloud();
    virtual void connect();
    virtual void write(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();

private:
    QTcpSocket connection;
    QString addres;
    quint16 port;
    
    QVector<Message> msgQue;

public slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void readyRead();

};

#endif
