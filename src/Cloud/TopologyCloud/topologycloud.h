#ifndef TOPOLOGYCLOUD_H
#define TOPOLOGYCLOUD_H

#include "Cloud/cloconnection.h"
#include <QTcpSocket>
#include "Message/message.h"


class Configurator;

//TODO: configurator to constructor

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
    virtual void writeConnected(QVector<Message>);
    virtual QVector<Message> readAll();
    virtual bool isBusy();

private:
    enum ReadingState { READ_key = 0,  READ_value = 1,  READ_timestamp = 3,  READ_max = 4};
    static int max_messages;
    static int max_messages_size;
    QVector<Message> messages_tosend;
    QVector<Message> messages_readed;
    QTcpSocket connection;
    bool isConnected();
    void send();
    QString addres;
    quint16 port;
    
    int messages_toread;
    QString read_values[3];
    int read_state;
    

public slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void readyRead();

signals:
    void orderReceived(QVector<Message>);

};

#endif
